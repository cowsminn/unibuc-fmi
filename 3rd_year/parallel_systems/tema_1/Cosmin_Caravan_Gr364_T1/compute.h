#ifndef COMPUTE_H
#define COMPUTE_H

float compute_denominator(float *x_loc, float *y_loc, int N, int nproc, int rank);

float compute_numerator(float *x_loc, float *y_loc, float *a_loc, int N, int nproc, int rank);

#endif /* COMPUTE_H */
