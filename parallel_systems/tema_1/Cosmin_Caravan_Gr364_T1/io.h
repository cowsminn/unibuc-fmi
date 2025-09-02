#ifndef IO_H
#define IO_H

int read_vector(float *v, int n, const char *filename);

int read_matrix(float *a, int m, int n, const char *filename);

void distribute_data(float *x, float *y, float *a, float *x_loc, float *y_loc, float *a_loc, int N, MPI_Comm comm);

void receive_data(float *x_loc, float *y_loc, float *a_loc, int N, MPI_Comm comm);

#endif /* IO_H */
