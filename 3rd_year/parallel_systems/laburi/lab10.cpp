#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <stdint.h>

// BitArray implementation in C
typedef struct {
    uint64_t* data;
    size_t bit_size;
    size_t array_size;
} BitArray;

BitArray* create_bitarray(size_t size) {
    BitArray* ba = (BitArray*)malloc(sizeof(BitArray));
    ba->bit_size = size;
    ba->array_size = (size + 63) / 64;
    ba->data = (uint64_t*)malloc(ba->array_size * sizeof(uint64_t));
    memset(ba->data, 0xFF, ba->array_size * sizeof(uint64_t));
    return ba;
}

void clear_bit(BitArray* ba, size_t pos) {
    ba->data[pos / 64] &= ~((uint64_t)1 << (pos % 64));
}

int test_bit(BitArray* ba, size_t pos) {
    return (ba->data[pos / 64] & ((uint64_t)1 << (pos % 64))) != 0;
}

void free_bitarray(BitArray* ba) {
    free(ba->data);
    free(ba);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    const int UPPER_BOUND = 10000;
    const int BLOCK_SIZE = 32 * 1024;
    
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    
    // Step 1: Find primes up to sqrt(n)
    int sqrt_n = (int)sqrt(UPPER_BOUND);
    BitArray* is_prime_small = create_bitarray(sqrt_n + 1);
    int* base_primes = (int*)malloc((sqrt_n / 2) * sizeof(int));
    int base_prime_count = 0;
    
    clear_bit(is_prime_small, 0);
    clear_bit(is_prime_small, 1);
    
    for (int i = 2; i * i <= sqrt_n; i++) {
        if (test_bit(is_prime_small, i)) {
            for (int j = i * i; j <= sqrt_n; j += i) {
                clear_bit(is_prime_small, j);
            }
        }
    }
    
    for (int i = 2; i <= sqrt_n; i++) {
        if (test_bit(is_prime_small, i)) {
            base_primes[base_prime_count++] = i;
        }
    }
    
    // Step 2: Segmented sieve with MPI
    int range_per_process = (UPPER_BOUND - sqrt_n) / size;
    int remainder = (UPPER_BOUND - sqrt_n) % size;
    
    int range_start = sqrt_n + 1 + rank * range_per_process + (rank < remainder ? rank : remainder);
    int range_end = range_start + range_per_process + (rank < remainder ? 1 : 0) - 1;
    if (range_end > UPPER_BOUND) range_end = UPPER_BOUND;
    
    // Dynamic array for local primes
    int local_capacity = 10000;
    int* local_primes = (int*)malloc(local_capacity * sizeof(int));
    int local_count = 0;
    
    // Add base primes for rank 0
    if (rank == 0) {
        memcpy(local_primes, base_primes, base_prime_count * sizeof(int));
        local_count = base_prime_count;
    }
    
    // Process blocks
    for (int block_start = range_start; block_start <= range_end; block_start += BLOCK_SIZE) {
        int block_end = block_start + BLOCK_SIZE - 1;
        if (block_end > range_end) block_end = range_end;
        
        BitArray* is_prime = create_bitarray(block_end - block_start + 1);
        
        // Mark composites
        for (int p = 0; p < base_prime_count; p++) {
            int prime = base_primes[p];
            int first = ((block_start + prime - 1) / prime) * prime;
            if (first == prime) first += prime;
            
            for (int j = first; j <= block_end; j += prime) {
                clear_bit(is_prime, j - block_start);
            }
        }
        
        // Collect primes
        for (int i = 0; i <= block_end - block_start; i++) {
            if (test_bit(is_prime, i)) {
                if (local_count >= local_capacity) {
                    local_capacity *= 2;
                    local_primes = (int*)realloc(local_primes, local_capacity * sizeof(int));
                }
                local_primes[local_count++] = block_start + i;
            }
        }
        
        free_bitarray(is_prime);
    }
    
    // Gather counts
    int* counts = NULL;
    if (rank == 0) {
        counts = (int*)malloc(size * sizeof(int));
    }
    MPI_Gather(&local_count, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate displacements
    int* displs = NULL;
    int total_count = 0;
    if (rank == 0) {
        displs = (int*)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            displs[i] = total_count;
            total_count += counts[i];
        }
    }
    
    // Gather all primes
    int* all_primes = NULL;
    if (rank == 0) {
        all_primes = (int*)malloc(total_count * sizeof(int));
    }
    
    MPI_Gatherv(local_primes, local_count, MPI_INT,
                all_primes, counts, displs, MPI_INT,
                0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Sort primes (using simple qsort)
        int compare(const void* a, const void* b) {
            return (*(int*)a - *(int*)b);
        }
        qsort(all_primes, total_count, sizeof(int), compare);
        
        double computation_time = MPI_Wtime() - start_time;
        
        // Save to text file
        double save_start = MPI_Wtime();
        FILE* file = fopen("primes_result.txt", "w");
        if (file) {
            fprintf(file, "Prime numbers between 2 and %d\n", UPPER_BOUND);
            fprintf(file, "Total count: %d\n", total_count);
            fprintf(file, "Number of processes: %d\n\n", size);
            
            for (int i = 0; i < total_count; i++) {
                fprintf(file, "%d\n", all_primes[i]);
            }
            fclose(file);
        }
        
        double save_time = MPI_Wtime() - save_start;
        double total_time = MPI_Wtime() - start_time;
        
        printf("\n=== FASTEST PRIME FINDER RESULTS ===\n");
        printf("Primes found: %d\n", total_count);
        printf("Range: 2 to %d\n", UPPER_BOUND);
        printf("MPI Processes: %d\n", size);
        printf("Computation time: %f seconds\n", computation_time);
        printf("File save time: %f seconds\n", save_time);
        printf("Total time: %f seconds\n", total_time);
        printf("Performance: %.0f numbers/second\n", UPPER_BOUND / computation_time);
        printf("File saved: primes_result.txt\n");
        
        free(all_primes);
        free(counts);
        free(displs);
    }
    
    free(local_primes);
    free(base_primes);
    free_bitarray(is_prime_small);
    
    MPI_Finalize();
    return 0;
}
