#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//mpicc -o x y.c
//mpirun -host localhost:3 ./x
int *create_send_buff(int size_send_buff){

    int *send_buff = (int*) malloc(size_send_buff*sizeof(int));

    for(int i = 0; i<size_send_buff;i++){
        send_buff[i] = i;
    }
    return send_buff;
}

void print_recv_buff(int *recv_buff,int size_recv_buff,int rank){

    printf("Node %d, ",rank);
    for(int i = 0; i <size_recv_buff;i++){
        printf("recv_buff[%d] = %d  ",i,recv_buff[i]);
    }
    printf("\n");
}

int main(int argc,char **argv){

    int num_procs, rank;
    int root = 0;
    int *recv_buff;
    int send_count[3] = {1,3,1};
    int displacements[3] = {0, 3, 7};

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    recv_buff = malloc(send_count[rank] * sizeof(int));

    if(rank == root){
        int* sendbuf = create_send_buff(8);
        MPI_Scatterv( sendbuf, send_count, displacements, MPI_INT, recv_buff, send_count[rank], MPI_INT, 
            root, MPI_COMM_WORLD); 
        free(sendbuf);
        print_recv_buff(recv_buff, send_count[rank], rank);
    }
    else{

        MPI_Scatterv(NULL, NULL, NULL, NULL, recv_buff, send_count[rank], MPI_INT, 
            root, MPI_COMM_WORLD); 
        
        print_recv_buff(recv_buff, send_count[rank], rank);
    }

    free(recv_buff);

    MPI_Finalize();


    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// int *create_send_buff(int size_send_buff, int rank){

//     int *send_buff = (int*) malloc(size_send_buff*sizeof(int));

//     for(int i = 0; i<size_send_buff;i++){
//         send_buff[i] = rank + i;
//     }
//     return send_buff;
// }

// void print_recv_buff(int *recv_buff,int size_recv_buff,int rank){

//     printf("Node %d, ",rank);
//     for(int i = 0; i <size_recv_buff;i++){
//         printf("recv_buff[%d] = %d\n",i,recv_buff[i]);
//     }
//     printf("\n");
// }

// int main(int argc,char **argv){

//     int num_procs, rank;
//     int root = 0;
//     int send_count[3] = {1,3,1};
//     int displacements[3] = {0, 3, 7};

//     MPI_Init(&argc,&argv);
//     MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
//     MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    

//     if(rank == root){
//         int sendbuf = 5;
//         int *recv_buff = calloc(15, sizeof(int));
//         MPI_Gatherv(&sendbuf, send_count[rank], MPI_INT, recv_buff, send_count, displacements, MPI_INT, 
//             root, MPI_COMM_WORLD);
        
//         print_recv_buff(recv_buff, 15, 0);
//         free(recv_buff);
//     }
//     else {
//         int* rez = create_send_buff(send_count[rank], rank);
//         MPI_Gatherv(rez, send_count[rank], MPI_INT, NULL, NULL, NULL, NULL,
//             root, MPI_COMM_WORLD);
//         free(rez);
//     }

//     MPI_Finalize();


//     return 0;
// }