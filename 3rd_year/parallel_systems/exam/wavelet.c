/* Programul asigura filtrarea semnalului de intrare din fisierul input.dat
 * utilizand transformata wavelet 1D implementata in biblioteca GSL. Sunt
 * utilizate pentru filtrare cele mai mari nc componente wavelet ale semnalului
 * initial, toate celelalte fiind anulate */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_wavelet.h>

int main (int argc, char *argv[])
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int i, n = 256, nc = 5; 
  double dummy;
  double *data = NULL;
  
  int nl = n / size;  
  double *ldata = malloc(nl * sizeof(double));

  double *abscoeff = malloc(nl * sizeof(double));
  
  size_t *lp = malloc(nl * sizeof(size_t));


  double *global_abscoeff = NULL;
  size_t *global_p = NULL;
  
  if (rank == 0) {
    data = malloc(n * sizeof(double));
    global_abscoeff = malloc(n * sizeof(double));
    global_p = malloc(n * sizeof(size_t));

    FILE *f = fopen("data.in", "r");
    if (!f) {
      printf("Error opening data.in\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    for (i = 0; i < n; i++) {
      fscanf(f, "%lg\t%lg", &dummy, &data[i]);
    }
    fclose(f);

    gsl_wavelet *w = gsl_wavelet_alloc(gsl_wavelet_daubechies, 4);
    gsl_wavelet_workspace *work = gsl_wavelet_workspace_alloc(n);
    gsl_wavelet_transform_forward(w, data, 1, n, work);

    for (i = 0; i < n; i++) {
      global_abscoeff[i] = fabs(data[i]);
    }

    gsl_sort_index(global_p, global_abscoeff, 1, n);

    for (i = 0; i < (n - nc); i++) {
      data[global_p[i]] = 0;
    }

    gsl_wavelet_transform_inverse(w, data, 1, n, work);
    
    gsl_wavelet_free(w);
    gsl_wavelet_workspace_free(work);
  }

  MPI_Scatter(data, nl, MPI_DOUBLE, ldata, nl, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Gather(ldata, nl, MPI_DOUBLE, data, nl, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    FILE *f = fopen("wavelet_signal.dat", "w");
    for (i = 0; i < n; i++) {
      fprintf(f, "%lg\t%lg\n", (double) i, data[i]);
    }
    fclose(f);
  }

  free(ldata);
  free(abscoeff);
  free(lp);
  
  if (rank == 0) {
    free(data);
    free(global_abscoeff);
    free(global_p);
  }
  
  MPI_Finalize();
  return 0;
}

