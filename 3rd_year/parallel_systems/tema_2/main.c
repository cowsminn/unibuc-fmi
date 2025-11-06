#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pgm_IO.h"
#include "pgm_IO.c"

int main(int argc, char *argv[]) {
    int M, N;
    int niter = 1000;
    char infile[256] = "image_640x480.pgm";
    char outfile[256] = "img";
    
    if (argc > 1) strcpy(infile, argv[1]);
    if (argc > 2) niter = atoi(argv[2]);
    if (argc > 3) strcpy(outfile, argv[3]);
    
    pgm_size(infile, &M, &N);
    printf("Image size: %d x %d pixels\n", M, N);
    
    float *data = (float *)malloc(M * N * sizeof(float));
    float *pold = (float *)malloc((M + 2) * (N + 2) * sizeof(float));
    float *pnew = (float *)malloc((M + 2) * (N + 2) * sizeof(float));
    float *plim = (float *)malloc((M + 2) * (N + 2) * sizeof(float));
    
    if (!data || !pold || !pnew || !plim) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }
    
    pgm_read(infile, data, M, N);
    
    for (int i = 0; i < (M + 2) * (N + 2); i++) {
        pold[i] = 255.0;
        pnew[i] = 255.0;
        plim[i] = 255.0;
    }
    
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            plim[i * (N + 2) + j] = data[(i - 1) * N + (j - 1)];
        }
    }
    
    printf("Starting reconstruction with %d iterations...\n", niter);
    
    for (int iter = 0; iter < niter; iter++) {
        for (int i = 1; i <= M; i++) {
            for (int j = 1; j <= N; j++) {
                int idx = i * (N + 2) + j;
                int idx_up = (i - 1) * (N + 2) + j;
                int idx_down = (i + 1) * (N + 2) + j;
                int idx_left = i * (N + 2) + (j - 1);
                int idx_right = i * (N + 2) + (j + 1);
                
                pnew[idx] = 0.25 * (pold[idx_up] + pold[idx_down] + 
                                   pold[idx_left] + pold[idx_right] - 
                                   plim[idx]);
            }
        }
        
        for (int i = 1; i <= M; i++) {
            for (int j = 1; j <= N; j++) {
                pold[i * (N + 2) + j] = pnew[i * (N + 2) + j];
            }
        }
        
        if ((iter + 1) % 100 == 0) {
            printf("Completed %d iterations\n", iter + 1);
        }
    }
    
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            data[(i - 1) * N + (j - 1)] = pold[i * (N + 2) + j];
        }
    }
    
    char final_outfile[256];
    sprintf(final_outfile, "%s_%d.pgm", outfile, niter);
    
    pgm_write(final_outfile, data, M, N);
    
    free(data);
    free(pold);
    free(pnew);
    free(plim);
    
    printf("Reconstruction complete. Output written to %s\n", final_outfile);
    
    return 0;
}