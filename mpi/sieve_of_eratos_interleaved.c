#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000 // 100M
#define UNMARKED 0
#define MARKED 1

int main(int argc, char *argv[]) {
    int id; // id of CPU
    int p; // number of processes
    int total_len = N - 1; // 2 to N
    char *nums;
    char *global_nums = NULL;
    int global_count = 0;
	double time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Barrier(MPI_COMM_WORLD); // stop to sync all processes

	time = -MPI_Wtime(); // start the timer

    nums = (char *)calloc(total_len, sizeof(char));

    if (nums == NULL) {
        printf("error while calloc nums\n");
        MPI_Finalize();
        exit(1);
    }

    if (id == 0) {
        global_nums = (char *)calloc(total_len, sizeof(char));
        if (global_nums == NULL) {
            printf("error while calloc global_nums\n");
            MPI_Finalize();
            exit(1);
        }
    }

    // k to N (2 = index 0, 3 = index 1, k = index k - 2)
    for (int k = id + 2; k * k <= N; k += p) { // cyclic decomposition (p0 for 2, 2+p, 2+2p ...) (load imbalance)

        // find the smallest number greater than k that is unmarked
        if (nums[k - 2] == MARKED) {
            continue;
        }

        // mark all multiples of k between kk and n
        for (int j = k * k; j <= N; j += k) {
            nums[j - 2] = MARKED;
        }

    }

    MPI_Reduce(nums, global_nums, total_len, MPI_CHAR, MPI_MAX, 0, MPI_COMM_WORLD);
	
    if (id == 0) {
        for (int i = 0; i < total_len; i++) {
            if (global_nums[i] == UNMARKED) {
                global_count++;
            }
        }
    }

    time += MPI_Wtime(); // stop the timer

    // printing
    if (id == 0) {
		printf("%d process, 2 to %d prime : %d primes, used %f s\n", p, N, global_count, time);
        free(global_nums);
    }

    free(nums);
    MPI_Finalize();

    return 0;

}

