#include <mpi.h>
float compute_numerator(int n, float* a_loc, float* x_loc, float* y) {
    float sum = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += x_loc[i] * a_loc[i * n + j] * y[j];
        }
    }
    return sum;
}
