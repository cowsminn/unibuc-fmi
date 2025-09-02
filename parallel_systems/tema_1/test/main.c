// numerator.c


// denominator.c


// main.c
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

extern void read_vector(const char* filename, float* vec, int n);
extern void read_matrix(const char* filename, float* mat, int n);
extern float compute_numerator(int n, float* a_loc, float* x_loc, float* y);
extern float compute_denominator(int n, float* x_loc, float* y_loc);

#define N 200

int main(int argc, char** argv) {
    int my_rank, nproc, my_new_rank;
    MPI_Comm low_comm, high_comm;
    MPI_Group world_group, low_group, high_group;

    float *a = NULL, *x = NULL, *y = NULL;
    float *a_loc, *x_loc, *y_loc;
    float mel = 0.0, mel_loc = 0.0, ps = 0.0, ps_loc = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    if (nproc % 2 != 0) {
        if (my_rank == 0) printf("Error: nproc must be even.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int half = nproc / 2;
    int* low_ranks = malloc(half * sizeof(int));
    int* high_ranks = malloc(half * sizeof(int));

    for (int i = 0; i < half; i++) {
        low_ranks[i] = i;
        high_ranks[i] = i + half;
    }

    if (my_rank < half) {
        MPI_Group_incl(world_group, half, low_ranks, &low_group);
        MPI_Comm_create(MPI_COMM_WORLD, low_group, &low_comm);
        if (low_comm != MPI_COMM_NULL) {
            MPI_Comm_rank(low_comm, &my_new_rank);

            if (my_new_rank == 0) {
                x = malloc(N * sizeof(float));
                y = malloc(N * sizeof(float));
                read_vector("x.dat", x, N);
                read_vector("y.dat", y, N);
            }

            x_loc = malloc((N/half) * sizeof(float));
            y_loc = malloc((N/half) * sizeof(float));

            MPI_Scatter(x, N/half, MPI_FLOAT, x_loc, N/half, MPI_FLOAT, 0, low_comm);
            MPI_Scatter(y, N/half, MPI_FLOAT, y_loc, N/half, MPI_FLOAT, 0, low_comm);

            ps_loc = compute_denominator(N/half, x_loc, y_loc);
            MPI_Reduce(&ps_loc, &ps, 1, MPI_FLOAT, MPI_SUM, 0, low_comm);

            if (my_new_rank == 0 && my_rank != 0) {
                MPI_Send(&ps, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            }

            free(x_loc); free(y_loc);
            if (my_new_rank == 0) { free(x); free(y); }
        }
    } else {
        MPI_Group_incl(world_group, half, high_ranks, &high_group);
        MPI_Comm_create(MPI_COMM_WORLD, high_group, &high_comm);
        if (high_comm != MPI_COMM_NULL) {
            MPI_Comm_rank(high_comm, &my_new_rank);

            if (my_new_rank == 0) {
                x = malloc(N * sizeof(float));
                y = malloc(N * sizeof(float));
                a = malloc(N * N * sizeof(float));
                read_vector("x.dat", x, N);
                read_vector("y.dat", y, N);
                read_matrix("mat.dat", a, N);
            }

            x_loc = malloc((N/half) * sizeof(float));
            a_loc = malloc((N/half) * N * sizeof(float));

            MPI_Scatter(x, N/half, MPI_FLOAT, x_loc, N/half, MPI_FLOAT, 0, high_comm);
            MPI_Scatter(a, (N/half)*N, MPI_FLOAT, a_loc, (N/half)*N, MPI_FLOAT, 0, high_comm);
            if (my_new_rank != 0) y = malloc(N * sizeof(float));
            MPI_Bcast(y, N, MPI_FLOAT, 0, high_comm);

            mel_loc = compute_numerator(N/half, a_loc, x_loc, y);
            MPI_Reduce(&mel_loc, &mel, 1, MPI_FLOAT, MPI_SUM, 0, high_comm);

            if (my_new_rank == 0 && my_rank != 0) {
                MPI_Send(&mel, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
            }

            free(x_loc); free(a_loc); if (y) free(y);
            if (my_new_rank == 0) { free(x); free(y); free(a); }
        }
    }

    if (my_rank == 0) {
        MPI_Recv(&ps, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mel, 1, MPI_FLOAT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        FILE* out = fopen("result.txt", "w");
        fprintf(out, "AVG = %f\n", mel/ps);
        fclose(out);
    }

    free(low_ranks); free(high_ranks);
    MPI_Finalize();
    return 0;
}

// Makefile


