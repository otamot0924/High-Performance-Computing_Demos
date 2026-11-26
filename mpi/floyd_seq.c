#include <mpi.h>
#include <stdio.h>

#define N 6 // number of vertices
#define INF 999999
#define MIN(a, b) ((a < b) ? a : b)

int main(int argc, char *argv[]) {

    int id; // id of CPU
    int p; // number of processes

    int A[] = {
        0,      2,      5,      INF,    INF,    INF,
        INF,    0,      7,      1,      INF,    8,
        INF,    INF,    0,      4,      INF,    INF,
        INF,    INF,    INF,    0,      3,      INF,
        INF,    INF,    2,      INF,    0,      3,
        INF,    5,      INF,    2,      4,      0
    }; // adjacency matrix

    // int *A = (int *) malloc(N * sizeof(int)); // adjacency matrix

    double time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Barrier(MPI_COMM_WORLD); // stop to sync all processes

    if (N % p != 0) {
        if (id == 0) {
            printf("N mod P != 0");
        }
        MPI_Finalize();
        return 0;
    }

	time = -MPI_Wtime(); // start the timer

    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] = MIN(A[i * N + j], A[i * N + k] + A[k * N + j]);
            }
        }
    }

    time += MPI_Wtime(); // stop the timer

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

    printf("%d process, %d vertices : used %f s\n", p, N, time);

    MPI_Finalize();

    return 0;

}