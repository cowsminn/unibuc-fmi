#include <mpi.h>
float compute_denominator(int n, float* x_loc, float* y_loc) {
    float sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += x_loc[i] * y_loc[i];
    }
    return sum;
}
