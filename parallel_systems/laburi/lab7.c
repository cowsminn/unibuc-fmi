#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, num_proc;
    MPI_Comm initial_comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(initial_comm, &rank);
    MPI_Comm_size(initial_comm, &num_proc);

//     int color = rank / 4;

//    // printf("Rank: %d, Color: %d\n", rank, color);

//     MPI_Comm row_comm;
//     MPI_Comm_split(initial_comm, color, rank, &row_comm);

//     int row_rank, row_num_procs;
//     MPI_Comm_rank(row_comm, &row_rank);
//     MPI_Comm_size(row_comm, &row_num_procs);

//     printf("initial com: rank/num procs: %d %d \t row com: rowrank/row num procs : %d %d \n", rank, num_proc, row_ranc, num_procs )


MPI_Group initial_group;
MPI_Comm_group(initial_comm, &initial_group);

// Define the prime ranks you want in the new group
int n = 7;
int ranks[7] = {1, 2, 3, 5, 7, 11, 13};

// Create new group and communicator
MPI_Group prime_group;
MPI_Group_incl(initial_group, n, ranks, &prime_group);

MPI_Comm prime_comm;
MPI_Comm_create_group(initial_comm, prime_group, 0, &prime_comm);

int prime_rank = -1, prime_num_procs = -1;
if (prime_comm != MPI_COMM_NULL) {
    MPI_Comm_rank(prime_comm, &prime_rank);
    MPI_Comm_size(prime_comm, &prime_num_procs);
}

printf("Initial comm: rank/size: %d/%d\t", rank, num_proc);
if (prime_comm != MPI_COMM_NULL)
    printf("Prime comm: rank/size: %d/%d\n", prime_rank, prime_num_procs);
else
    printf("Not in prime group\n");

MPI_Finalize();
return 0;
}
