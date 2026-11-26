#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// #define N 6 // number of vertices
#define INF 999999
#define MIN(a, b) ((a < b) ? a : b)

#define BLOCK_LOW(id, p, n)  ((int)(((long long)(id) * (n)) / (p))) // first element controlled by process i
#define BLOCK_HIGH(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - 1) // last element controlled by process i = first element controlled by process i + 1
#define BLOCK_SIZE(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - BLOCK_LOW(id, p, n) )
#define BLOCK_OWNER(j, p, n) ((int)((((long long)(p) * ((j) + 1) - 1)) / (n)))

void gen_rand(int *A, int n) {

    srand(42); // rand seed

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                A[i * n + j] = 0;
            } else if ((rand() % 100) < 30) { // 30% probability has path
                A[i * n + j] = (rand() % 100) + 1; // weight 1 ~ 100
            } else {
                A[i * n + j] = INF;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    int id; // id of CPU
    int p; // number of processes
    double time;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int N = (argc > 1) ? atoi(argv[1]) : 1000; // command input arg or 1000

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

        A = (int *)malloc(N * N * sizeof(int));
        gen_rand(A, N);
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
            int dik = local_A[(size_t)i * N + k];
            if (dik == INF) continue;

            for (int j = 0; j < N; j++) { // col
                if (k_row[j] != INF) {
                    local_A[(size_t)i * N + j] = MIN(local_A[(size_t)i * N + j], dik + k_row[j]);
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

        long long checksum = 0;

        for (size_t i = 0; i < (size_t)N * N; i++) {
            if (A[i] != INF) {
                checksum += A[i];
            }
        }

        printf("%2d processes, %4d vertices : time used %8.4f s, checksum: %lld\n", p, N, time, checksum);
        free(A);
    }

    free(local_A);
    free(k_row);

    MPI_Finalize();

    return 0;

}

// run: mpiexec -n <process num> .exe <vertices num>