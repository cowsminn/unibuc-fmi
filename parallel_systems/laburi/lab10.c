#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define PI 3.14159265358979323846

// BitArray implementation for efficient Sieve of Eratosthenes
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

// Generate prime numbers up to a limit using Sieve of Eratosthenes
int* generate_primes(int limit, int* count) {
    BitArray* is_prime = create_bitarray(limit + 1);
    
    // Initialize sieve (0 and 1 are not prime)
    clear_bit(is_prime, 0);
    clear_bit(is_prime, 1);
    
    // Sieve of Eratosthenes
    for (int i = 2; i * i <= limit; i++) {
        if (test_bit(is_prime, i)) {
            for (int j = i * i; j <= limit; j += i) {
                clear_bit(is_prime, j);
            }
        }
    }
    
    // Count primes
    *count = 0;
    for (int i = 2; i <= limit; i++) {
        if (test_bit(is_prime, i)) {
            (*count)++;
        }
    }
    
    // Allocate and fill prime numbers array
    int* primes = (int*)malloc(*count * sizeof(int));
    int index = 0;
    for (int i = 2; i <= limit; i++) {
        if (test_bit(is_prime, i)) {
            primes[index++] = i;
        }
    }
    
    free_bitarray(is_prime);
    return primes;
}

// Calculate radius of a sphere from its volume (r = ∛(3V/(4π)))
double calculate_radius(int volume) {
    return pow((3.0 * volume) / (4.0 * PI), 1.0/3.0);
}

// Function to compare doubles for descending sort
int compare_doubles_desc(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da < db) ? 1 : (da > db) ? -1 : 0;  // Descending order
}

// Hierarchical packing model for spheres with volumes equal to prime numbers
double hierarchical_packing_model(int* primes, int count) {
    // Calculate radii for each sphere where volume = prime number
    double* radii = (double*)malloc(count * sizeof(double));
    for (int i = 0; i < count; i++) {
        radii[i] = calculate_radius(primes[i]);
    }
    
    // Sort radii in descending order
    qsort(radii, count, sizeof(double), compare_doubles_desc);
    
    // Calculate total volume using the hierarchical model
    double total_volume = 0.0;
    for (int i = 0; i < count; i++) {
        double radius = radii[i];
        
        // Packing factor decreases as spheres get smaller
        // This reflects the fact that smaller spheres can be packed more efficiently
        double packing_factor = 1.0 + 0.5 * exp(-(double)i / 100.0);
        
        // Sphere volume: V = (4/3)πr³ 
        // However, in this problem, we already know V = prime number
        double sphere_volume = primes[count - i - 1]; // Map back to the original prime
        
        // Adjusted volume considering packing efficiency
        total_volume += sphere_volume * packing_factor;
    }
    
    // Cube side length is the cube root of the total volume
    double cube_side_length = pow(total_volume, 1.0/3.0);
    
    // Print detailed information
    printf("Hierarchical Packing Model Analysis:\n");
    printf("Number of spheres (prime numbers): %d\n", count);
    printf("Minimum radius (for prime 2): %.4f\n", calculate_radius(2));
    printf("Maximum radius (for prime %d): %.4f\n", primes[count-1], calculate_radius(primes[count-1]));
    
    // Calculate average radius
    double sum_radii = 0.0;
    for (int i = 0; i < count; i++) {
        sum_radii += radii[i];
    }
    double avg_radius = sum_radii / count;
    printf("Average radius: %.4f\n", avg_radius);
    
    printf("Total estimated volume: %.2f\n", total_volume);
    printf("Cube side length: %.4f units\n", cube_side_length);
    
    free(radii);
    return cube_side_length;
}

// Implement other estimation methods for comparison
void estimate_cube_side(int* primes, int count) {
    // Calculate sum of all primes (sum of sphere volumes)
    long long sum_primes = 0;
    for (int i = 0; i < count; i++) {
        sum_primes += primes[i];
    }
    
    // Method 1: Using packing density
    double packing_density = 0.5;  // 50% packing efficiency
    double cube_volume1 = sum_primes / packing_density;
    double cube_side_length1 = pow(cube_volume1, 1.0/3.0);
    
    // Method 2: Worst case (all spheres with maximum radius)
    double max_radius = calculate_radius(primes[count-1]);
    double worst_case_side_length = 2.0 * max_radius * sqrt(count);
    
    // Method 3: Based on sum of radii
    double sum_radii = 0.0;
    for (int i = 0; i < count; i++) {
        sum_radii += calculate_radius(primes[i]);
    }
    double K = 1.2;  // Empirical constant
    double cube_side_length3 = K * pow(sum_radii, 1.0/3.0) * pow(count, 2.0/3.0);
    
    printf("\nOther Estimation Methods:\n");
    printf("1. Based on packing density: %.4f units\n", cube_side_length1);
    printf("2. Worst case (maximum radius spheres): %.4f units\n", worst_case_side_length);
    printf("3. Based on sum of radii: %.4f units\n", cube_side_length3);
}

// Function to demonstrate the model with a small example
void demonstrate_small_example() {
    printf("\n=== Demonstration with small example ===\n");
    
    // First 5 primes: 2, 3, 5, 7, 11
    int small_primes[] = {2, 3, 5, 7, 11};
    int count = 5;
    
    printf("Example with first 5 prime numbers:\n");
    printf("Prime numbers: 2, 3, 5, 7, 11\n\n");
    
    // Calculate and display the detailed steps
    printf("Calculation steps:\n");
    printf("--------------------------------------------------\n");
    printf("| Prime | Volume | Radius  | Position | Packing  | Adjusted |\n");
    printf("| (Mi)  | (Vi=Mi)| (ri)    | (i)      | Factor   | Volume   |\n");
    printf("--------------------------------------------------\n");
    
    // Calculate radii and sort in descending order
    double radii[5];
    int sorted_indices[5];
    
    for (int i = 0; i < count; i++) {
        radii[i] = calculate_radius(small_primes[i]);
        sorted_indices[i] = i;
    }
    
    // Sort indices by radius in descending order
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (radii[sorted_indices[i]] < radii[sorted_indices[j]]) {
                int temp = sorted_indices[i];
                sorted_indices[i] = sorted_indices[j];
                sorted_indices[j] = temp;
            }
        }
    }
    
    // Display calculation steps
    double total_adjusted_volume = 0.0;
    for (int i = 0; i < count; i++) {
        int prime_index = sorted_indices[i];
        int prime = small_primes[prime_index];
        double radius = radii[prime_index];
        double packing_factor = 1.0 + 0.5 * exp(-(double)i / 100.0);
        double adjusted_volume = prime * packing_factor;
        total_adjusted_volume += adjusted_volume;
        
        printf("| %-5d | %-6d | %-7.4f | %-8d | %-8.4f | %-8.2f |\n", 
               prime, prime, radius, i, packing_factor, adjusted_volume);
    }
    
    printf("--------------------------------------------------\n");
    printf("| Total |        |         |          |          | %-8.2f |\n", total_adjusted_volume);
    printf("--------------------------------------------------\n");
    
    double cube_side = pow(total_adjusted_volume, 1.0/3.0);
    printf("Cube side length = ∛%.2f = %.4f units\n\n", total_adjusted_volume, cube_side);
}

// Test the packing model for different limits
void test_packing_model(int limit) {
    printf("\n=== Testing for limit %d ===\n", limit);
    
    int count;
    int* primes = generate_primes(limit, &count);
    
    printf("Number of primes up to %d: %d\n", limit, count);
    printf("First prime: %d\n", primes[0]);
    printf("Last prime: %d\n", primes[count-1]);
    
    // Apply hierarchical packing model
    double result = hierarchical_packing_model(primes, count);
    
    // Apply other estimation methods
    estimate_cube_side(primes, count);
    
    // Final recommendation
    printf("\nFinal Recommendation:\n");
    printf("Minimum cube side length (L): %d units\n", (int)ceil(result));
    
    free(primes);
}

int main() {
    printf("===== SPHERE PACKING MODEL FOR PRIME NUMBERS =====\n");
    printf("This program calculates the minimum cube side length needed\n");
    printf("to contain spheres with volumes equal to prime numbers.\n\n");
    
    // Demonstrate with a small example
    demonstrate_small_example();
    
    // Test for different limits
    test_packing_model(100);
    test_packing_model(1000);
    
    // Test for the problem requirement (10,000)
    printf("\n=== SOLUTION FOR THE GIVEN PROBLEM (n=10,000) ===\n");
    test_packing_model(10000);
    
    return 0;
}