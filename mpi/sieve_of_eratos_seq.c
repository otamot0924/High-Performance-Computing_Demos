#include <mpi.h>
#include <stdio.h>

#define N 60
#define UNMARKED 0
#define MARKED 1

int main(int argc, char *argv[]) {
    int k = 2;
    int nums[N - 1] = {UNMARKED}; // 2 to N
    int global_nums[N - 1]; // 2 to N
    int id; // id of CPU
    int p; // number of processes
	double time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

	time = -MPI_Wtime(); // start the timer

    // k to N (2 = index 0, 3 = index 1, k = index k - 2)
    for (k = 2; k * k <= N; k++) {

        // find the smallest number greater than k that is unmarked
        if (nums[k - 2] == MARKED) {
            continue;
        }

        // mark all multiples of k between kk and n
        for (int j = k * k; j <= N; j += k) {
            nums[j - 2] = MARKED;
        }

    }

    // MPI_Reduce(&nums, &global_nums, 1, MPI_INT, MPI_MAX, )
	time += MPI_Wtime(); // stop the timer

    // printing
    if (id == 0) {
		printf("%d process, 2 to %d prime : used %f s\n", p, N, time);
        for (k = 2; k <= N; k++) {
            if (nums[k - 2] == UNMARKED) {
                printf("%d ", k);
            }
        }
    }

    MPI_Finalize();

    return 0;

}

