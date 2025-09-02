#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, size;
    int send_data[10], recv_data[10];
    int i;
    
    // (d) Inițializare MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // (a) Inițializare siruri - primul cu valori succesive începând cu rangul procesului
    // al doilea cu 0
    for (i = 0; i < 10; i++) {
        send_data[i] = rank + i;
        recv_data[i] = 0;
    }
    
    printf("Procesul %d: send_data initial = [", rank);
    for (i = 0; i < 10; i++) {
        printf("%d", send_data[i]);
        if (i < 9) printf(", ");
    }
    printf("]\n");
    
    printf("Procesul %d: recv_data initial = [", rank);
    for (i = 0; i < 10; i++) {
        printf("%d", recv_data[i]);
        if (i < 9) printf(", ");
    }
    printf("]\n");
    
    // (b) Pe o topologie inelară, fiecare proces trimite către procesul de rang imediat superior
    // valorile din send_data și recepționează în recv_data valorile de la procesul cu rang inferior
    int next_rank = (rank + 1) % size;  // rangul procesului următor în inel
    int prev_rank = (rank - 1 + size) % size;  // rangul procesului anterior în inel
    
    MPI_Sendrecv(send_data, 10, MPI_INT, next_rank, 0,
                 recv_data, 10, MPI_INT, prev_rank, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    printf("Procesul %d: recv_data dupa Sendrecv = [", rank);
    for (i = 0; i < 10; i++) {
        printf("%d", recv_data[i]);
        if (i < 9) printf(", ");
    }
    printf("]\n");
    
    // (c) Procesul cu rang 0 afișează la final conținutul sirurilor send_data și recv_data
    if (rank == 0) {
        printf("\n=== PROCESUL 0 - REZULTATE FINALE ===\n");
        printf("send_data[10] = [");
        for (i = 0; i < 10; i++) {
            printf("%d", send_data[i]);
            if (i < 9) printf(", ");
        }
        printf("]\n");
        
        printf("recv_data[10] = [");
        for (i = 0; i < 10; i++) {
            printf("%d", recv_data[i]);
            if (i < 9) printf(", ");
        }
        printf("]\n");
    }
    
    // (d) Finalizare MPI
    MPI_Finalize();
    return 0;
}