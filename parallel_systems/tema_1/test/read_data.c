#include <stdio.h>
#include <stdlib.h>

void read_vector(const char* filename, float* vec, int n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        fscanf(f, "%f", &vec[i]);
    }
    fclose(f);
}

void read_matrix(const char* filename, float* mat, int n) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n * n; i++) {
        fscanf(f, "%f", &mat[i]);
    }
    fclose(f);
}

