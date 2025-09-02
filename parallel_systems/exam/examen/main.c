#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Comm comm_g1, comm_g2;
    MPI_Group world_group, group_g1, group_g2;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size % 2 != 0) {
        if (rank == 0) {
            printf("Error: Number of processes must be even!\n");
        }
        MPI_Finalize();
        return -1;
    }

    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int *ranks_g1 = malloc(sizeof(int) * size);
    int *ranks_g2 = malloc(sizeof(int) * size);
    int size_g1 = 0, size_g2 = 0;
    
    for (int i = 0; i < size; i++) {
        if (i % 2 == 0) {
            ranks_g1[size_g1++] = i;
        } else {
            ranks_g2[size_g2++] = i;
        }
    }
    
    MPI_Group_incl(world_group, size_g1, ranks_g1, &group_g1);
    MPI_Group_incl(world_group, size_g2, ranks_g2, &group_g2);
    
    MPI_Comm_create(MPI_COMM_WORLD, group_g1, &comm_g1);
    MPI_Comm_create(MPI_COMM_WORLD, group_g2, &comm_g2);
    
    if (rank % 2 == 0) {
        if (comm_g1 != MPI_COMM_NULL) {
            int g1_rank, g1_size;
            MPI_Comm_rank(comm_g1, &g1_rank);
            MPI_Comm_size(comm_g1, &g1_size);
            
            float *data = NULL;
            int N = 0;

            if (g1_rank == 0) {
                FILE *file = fopen("input1.dat", "r");
                if (!file) {
                    printf("Error opening input1.dat\n");
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                
                fscanf(file, "%d", &N);
                data = malloc(sizeof(float) * N);
                for (int i = 0; i < N; i++) {
                    fscanf(file, "%f", &data[i]);
                }
                fclose(file);
            }

            MPI_Bcast(&N, 1, MPI_INT, 0, comm_g1);
            
            if (g1_rank != 0) {
                data = malloc(sizeof(float) * N);
            }
            MPI_Bcast(data, N, MPI_FLOAT, 0, comm_g1);

            double local_sum = 0.0;
            int elements_per_process = N / g1_size;
            int remainder = N % g1_size;
            int start = g1_rank * elements_per_process + (g1_rank < remainder ? g1_rank : remainder);
            int end = start + elements_per_process + (g1_rank < remainder ? 1 : 0);
            
            for (int i = start; i < end; i++) {
                double xi = data[i];
                local_sum += (xi * xi) / (xi * xi + 1) * exp(-xi * xi);
            }

            double total_sum;
            MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm_g1);
            
            // Write result
            if (g1_rank == 0) {
                FILE *output = fopen("output1.dat", "w");
                fprintf(output, "N:\t%d val [NEWLINE]\n", N);
                fprintf(output, "Suma ponderată:\t%.6f val [NEWLINE]\n", total_sum);
                fclose(output);
            }
            
            free(data);
        }
    } else {
        if (comm_g2 != MPI_COMM_NULL) {
            int g2_rank, g2_size;
            MPI_Comm_rank(comm_g2, &g2_rank);
            MPI_Comm_size(comm_g2, &g2_size);
            
            float *data = NULL;
            int N = 0;
            
            if (g2_rank == 0) {
                FILE *file = fopen("input2.dat", "r");
                if (!file) {
                    printf("Error opening input2.dat\n");
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                
                fscanf(file, "%d", &N);
                data = malloc(sizeof(float) * N);
                for (int i = 0; i < N; i++) {
                    fscanf(file, "%f", &data[i]);
                }
                fclose(file);
            }
            
    
            MPI_Bcast(&N, 1, MPI_INT, 0, comm_g2);
            
            if (g2_rank != 0) {
                data = malloc(sizeof(float) * N);
            }
            MPI_Bcast(data, N, MPI_FLOAT, 0, comm_g2);
            
            
            double local_sum = 0.0;
            int elements_per_process = N / g2_size;
            int remainder = N % g2_size;
            int start = g2_rank * elements_per_process + (g2_rank < remainder ? g2_rank : remainder);
            int end = start + elements_per_process + (g2_rank < remainder ? 1 : 0);
            
            for (int i = start; i < end; i++) {
                double xi = data[i];
                if (xi != 0) { 
                    local_sum += xi / log(1 + xi * xi) * exp(-xi * xi);
                }
            }
            
            double total_sum;
            MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm_g2);

            if (g2_rank == 0) {
                FILE *output = fopen("output2.dat", "w");
                fprintf(output, "N:\t%d val [NEWLINE]\n", N);
                fprintf(output, "Suma ponderată:\t%.6f val [NEWLINE]\n", total_sum);
                fclose(output);
            }
            
            free(data);
        }
    }
    
    if (comm_g1 != MPI_COMM_NULL) MPI_Comm_free(&comm_g1);
    if (comm_g2 != MPI_COMM_NULL) MPI_Comm_free(&comm_g2);
    MPI_Group_free(&group_g1);
    MPI_Group_free(&group_g2);
    MPI_Group_free(&world_group);
    free(ranks_g1);
    free(ranks_g2);
    
    MPI_Finalize();
    return 0;
}
