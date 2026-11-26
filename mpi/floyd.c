#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 6 // number of vertices
#define INF 999999
#define MIN(a, b) ((a < b) ? a : b)

#define BLOCK_LOW(id, p, n)  ( (id) * (n) / (p) ) // first element controlled by process i
#define BLOCK_HIGH(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - 1) // last element controlled by process i = first element controlled by process i + 1
#define BLOCK_SIZE(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - BLOCK_LOW(id, p, n) )
#define BLOCK_OWNER(index, p, n) ( ( (p) * ((index) + 1) - 1) / n )

int main(int argc, char *argv[]) {

    int id; // id of CPU
    int p; // number of processes
    double time;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (N % p != 0) {
        if (id == 0) {
            printf("N mod P != 0");
        }
        MPI_Finalize();
        return 0;
    }

    int local_rows = BLOCK_SIZE(id, p, N); // rows for local
    int low_row = BLOCK_LOW(id, p, N);

    int *A = NULL; // adjacency matrix
    int *local_A = (int *)malloc(local_rows * N * sizeof(int));
    int *k_row = (int *)malloc(N * sizeof(int));

    if (id == p - 1) { // last process prepare matrix

        int init_A[N * N] = {
            0,      2,      5,      INF,    INF,    INF,
            INF,    0,      7,      1,      INF,    8,
            INF,    INF,    0,      4,      INF,    INF,
            INF,    INF,    INF,    0,      3,      INF,
            INF,    INF,    2,      INF,    0,      3,
            INF,    5,      INF,    2,      4,      0
        };

        A = (int *)malloc(N * N * sizeof(int));

        for (int i = 0; i < N * N; i++) {
            A[i] = init_A[i];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // stop to sync all processes
	time = -MPI_Wtime(); // start the timer

    if (id == p - 1) { // last process distributes data
        
        // send to front processes (0 ~ p - 2)
        for (int dest = 0; dest < p - 1; dest++) {
            int dest_low = BLOCK_LOW(dest, p, N);
            int dest_rows = BLOCK_SIZE(dest, p, N);
            MPI_Send(&A[dest_low * N], dest_rows * N, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        // last block for last process
        for (int i = 0; i < local_rows * N; i++) {
            local_A[i] = A[low_row * N + i];
        }
    }
    else { // other receive
        MPI_Recv(local_A, local_rows * N, MPI_INT, p - 1, 0, MPI_COMM_WORLD, &status);
    }

    // floyd
    for (int k = 0; k < N; k++) { // a circle = a task

        int owner = BLOCK_OWNER(k, p, N);

        if (id == owner) {
            
            int local_k = k - low_row;

            for (int j = 0; j < N; j++) {
                k_row[j] = local_A[local_k * N + j];
            }

            for (int dest = 0; dest < p; dest++) {
                if (dest != id) {
                    MPI_Send(k_row, N, MPI_INT, dest, k, MPI_COMM_WORLD);
                }
            }
        }
        else {
            MPI_Recv(k_row, N, MPI_INT, owner, k, MPI_COMM_WORLD, &status);
        }

        // update local rows
        for (int i = 0; i < local_rows; i++) { // row
            for (int j = 0; j < N; j++) { // col
                if (local_A[i * N + k] != INF && k_row[j] != INF) {
                    local_A[i * N + j] = MIN(local_A[i * N + j], local_A[i * N + k] + k_row[j]);
                }
            }
        }
    }
    
    if (id == p - 1) { // last process collects results

        for (int src = 0; src < p - 1; src++) {
            int src_low = BLOCK_LOW(src, p, N);
            int src_rows = BLOCK_SIZE(src, p, N);
            MPI_Recv(&A[src_low * N], src_rows * N, MPI_INT, src, 1, MPI_COMM_WORLD, &status);
        }

        for (int i = 0; i < local_rows * N; i++) {
            A[low_row * N + i] = local_A[i];
        }
    }
    else { // other send result
        MPI_Send(local_A, local_rows * N, MPI_INT, p - 1, 1, MPI_COMM_WORLD);
    }

    time += MPI_Wtime(); // stop the timer

    if (id == p - 1) { // last process prints result
        
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (A[i * N + j] == INF) {
                    printf("oo\t");
                }
                else {
                    printf("%d\t", A[i * N + j]);
                }
            }
            printf("\n");
        }

        printf("%d process, %d vertices : time used %f s\n", p, N, time);
        free(A);
    }

    free(local_A);
    free(k_row);

    MPI_Finalize();

    return 0;

}