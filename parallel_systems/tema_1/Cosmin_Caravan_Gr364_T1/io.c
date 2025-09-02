#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "io.h"

int read_vector(float *v, int n, const char *filename) {
    FILE *fp;
    int i;
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "err: fisier %s \n", filename);
        return 1;
    }

    for (i = 0; i < n; i++) {
        if (fscanf(fp, "%f", &v[i]) != 1) {
            fprintf(stderr, "err: %d din %s.\n", i, filename);
            fclose(fp);
            return 2;
        }
    }
    
    fclose(fp);
    
    return 0;
}

int read_matrix(float *a, int m, int n, const char *filename) {
    FILE *fp;
    int i, j;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "err: fisier %s \n", filename);
        return 1;
    }
    
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (fscanf(fp, "%f", &a[i * n + j]) != 1) {
                fprintf(stderr, "err: (%d,%d) din %s.\n", i, j, filename);
                fclose(fp);
                return 2;
            }
        }
    }
    
    fclose(fp);
    
    return 0;
}

void distribute_data(float *x, float *y, float *a, float *x_loc, float *y_loc, float *a_loc, int N, MPI_Comm comm) {
    int my_rank, nproc;
    int i, local_n, offset;
    int *sendcounts, *displs;
    
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &nproc);
    
    sendcounts = (int *)malloc(nproc * sizeof(int));
    displs = (int *)malloc(nproc * sizeof(int));

    offset = 0;
    for (i = 0; i < nproc; i++) {
        local_n = N / nproc + (i < N % nproc ? 1 : 0);
        sendcounts[i] = local_n;
        displs[i] = offset;
        offset += local_n;
    }

    if (a != NULL) {
        MPI_Bcast(x, N, MPI_FLOAT, 0, comm);
        MPI_Bcast(y, N, MPI_FLOAT, 0, comm);
        
        for (i = 0; i < N; i++) {
            x_loc[i] = x[i];
            y_loc[i] = y[i];
        }

        int *a_sendcounts = malloc(nproc * sizeof(int));
        int *a_displs = malloc(nproc * sizeof(int));
        
        for (i = 0; i < nproc; i++) {
            a_sendcounts[i] = sendcounts[i] * N;
            a_displs[i] = displs[i] * N;
        }

        MPI_Scatterv(a, a_sendcounts, a_displs, MPI_FLOAT, a_loc, a_sendcounts[my_rank], MPI_FLOAT, 0, comm);
        
        free(a_sendcounts);
        free(a_displs);
    } 
    else {
    
        local_n = sendcounts[my_rank];
   
        MPI_Scatterv(x, sendcounts, displs, MPI_FLOAT, x_loc, local_n, MPI_FLOAT, 0, comm);
        MPI_Scatterv(y, sendcounts, displs, MPI_FLOAT, y_loc, local_n, MPI_FLOAT, 0, comm);
    }

    free(sendcounts);
    free(displs);
}

void receive_data(float *x_loc, float *y_loc, float *a_loc, int N, MPI_Comm comm) {
    int my_rank, nproc;
    int local_n;
    
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &nproc);
    
    local_n = N / nproc + (my_rank < N % nproc ? 1 : 0);
    
    if (a_loc != NULL) {
        float *x = (float *)malloc(N * sizeof(float));
        float *y = (float *)malloc(N * sizeof(float));
        
        MPI_Bcast(x, N, MPI_FLOAT, 0, comm);
        MPI_Bcast(y, N, MPI_FLOAT, 0, comm);

        for (int i = 0; i < N; i++) {
            x_loc[i] = x[i];
            y_loc[i] = y[i];
        }

        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, a_loc, local_n * N, MPI_FLOAT, 0, comm);

        free(x);
        free(y);
    } 
    else {
        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, x_loc, local_n, MPI_FLOAT, 0, comm);
        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, y_loc, local_n, MPI_FLOAT, 0, comm);
    }
}
