#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "io.h"
#include "compute.h"

int main(int argc, char **argv) {
    int my_rank, my_new_rank, nproc;
    int *low_ranks, *high_ranks;
    float *a = NULL, *x = NULL, *y = NULL;
    float *a_loc = NULL, *x_loc = NULL, *y_loc = NULL;
    float mel = 0.0, mel_loc = 0.0, ps = 0.0, ps_loc = 0.0;
    MPI_Comm world_comm = MPI_COMM_WORLD, low_comm, high_comm;
    MPI_Group world_group, low_group, high_group;
    const int N = 200; 
    int i, new_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(world_comm, &nproc);
    MPI_Comm_rank(world_comm, &my_rank);
    
    if (nproc % 2 != 0) {
        if (my_rank == 0) {
            printf("err: nr de procese impare\n");
        }
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_group(world_comm, &world_group);

    low_ranks = (int *)malloc(nproc/2 * sizeof(int));
    high_ranks = (int *)malloc(nproc/2 * sizeof(int));

    for (i = 0; i < nproc/2; i++) {
        low_ranks[i] = i;
        high_ranks[i] = i + nproc/2;
    }

    MPI_Group_incl(world_group, nproc/2, low_ranks, &low_group);
    MPI_Group_incl(world_group, nproc/2, high_ranks, &high_group);

    MPI_Comm_create(world_comm, low_group, &low_comm);
    MPI_Comm_create(world_comm, high_group, &high_comm);

    free(low_ranks);
    free(high_ranks);

    if (my_rank < nproc/2) {
        MPI_Group_rank(low_group, &my_new_rank);
        MPI_Comm_size(low_comm, &new_size);

        x_loc = (float *)malloc((N / new_size + (my_new_rank < N % new_size ? 1 : 0)) * sizeof(float));
        y_loc = (float *)malloc((N / new_size + (my_new_rank < N % new_size ? 1 : 0)) * sizeof(float));
        
        if (my_new_rank == 0){
            x = (float *)malloc(N * sizeof(float));
            y = (float *)malloc(N * sizeof(float));
            
            read_vector(x, N, "x.dat");
            read_vector(y, N, "y.dat");
            
            distribute_data(x, y, NULL, x_loc, y_loc, NULL, N, low_comm);

            free(x);
            free(y);
        } else {
            receive_data(x_loc, y_loc, NULL, N, low_comm);
        }

        ps_loc = compute_denominator(x_loc, y_loc, N, new_size, my_new_rank);

        MPI_Reduce(&ps_loc, &ps, 1, MPI_FLOAT, MPI_SUM, 0, low_comm);

        if (my_new_rank == 0 && my_rank != 0) {
            MPI_Send(&ps, 1, MPI_FLOAT, 0, 0, world_comm);
        }

        free(x_loc);
        free(y_loc);

        MPI_Group_free(&low_group);
        MPI_Comm_free(&low_comm);
    } else {
        MPI_Group_rank(high_group, &my_new_rank);
        MPI_Comm_size(high_comm, &new_size);

        int local_rows = N / new_size + (my_new_rank < N % new_size ? 1 : 0);
        a_loc = (float *)malloc(local_rows * N * sizeof(float));
        x_loc = (float *)malloc(N * sizeof(float)); 
        y_loc = (float *)malloc(N * sizeof(float));  
        
        if (my_new_rank == 0) {
            a = (float *)malloc(N * N * sizeof(float));
            x = (float *)malloc(N * sizeof(float));
            y = (float *)malloc(N * sizeof(float));
            
            read_matrix(a, N, N, "mat.dat");
            read_vector(x, N, "x.dat");
            read_vector(y, N, "y.dat");

            distribute_data(x, y, a, x_loc, y_loc, a_loc, N, high_comm);

            free(a);
            free(x);
            free(y);
        } else {
            receive_data(x_loc, y_loc, a_loc, N, high_comm);
        }

        mel_loc = compute_numerator(x_loc, y_loc, a_loc, N, new_size, my_new_rank);

        MPI_Reduce(&mel_loc, &mel, 1, MPI_FLOAT, MPI_SUM, 0, high_comm);

        if (my_new_rank == 0) {
            MPI_Send(&mel, 1, MPI_FLOAT, 0, 1, world_comm);
        }

        free(a_loc);
        free(x_loc);
        free(y_loc);

        MPI_Group_free(&high_group);
        MPI_Comm_free(&high_comm);
    }

    if (my_rank == 0) {
        float avg;

        MPI_Recv(&mel, 1, MPI_FLOAT, nproc/2, 1, world_comm, MPI_STATUS_IGNORE);
        
        avg = mel / ps;
        
        FILE *fp = fopen("result.txt", "w");
        if (fp != NULL) {
            fprintf(fp, "Avg = %f\n", avg);
            fclose(fp);
            printf("salvat in result.txt: Avg = %f\n", avg);
        } else {
            printf("err: fisier outpu\n");
        }
    }
    
    MPI_Group_free(&world_group);
    
    MPI_Finalize();
    
    return 0;
}
