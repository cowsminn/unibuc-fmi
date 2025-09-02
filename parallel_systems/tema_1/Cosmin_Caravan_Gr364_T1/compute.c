#include "compute.h"

float compute_denominator(float *x_loc, float *y_loc, int N, int nproc, int rank) {
    float sum = 0.0;
    int local_n, i;

    local_n = N / nproc + (rank < N % nproc ? 1 : 0);

    for (i = 0; i < local_n; i++) {
        sum += x_loc[i] * y_loc[i];
    }
    
    return sum;
}

float compute_numerator(float *x_loc, float *y_loc, float *a_loc, int N, int nproc, int rank) {
    float sum = 0.0;
    int local_n, i, j;
    int global_i;

    local_n = N / nproc + (rank < N % nproc ? 1 : 0);

    int start_idx = 0;
    for (i = 0; i < rank; i++) {
        start_idx += N / nproc + (i < N % nproc ? 1 : 0);
    }

    for (i = 0; i < local_n; i++) {
        global_i = start_idx + i;
        for (j = 0; j < N; j++) {
            sum += x_loc[global_i] * a_loc[i * N + j] * y_loc[j];
        }
    }
    
    return sum;
}
