#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000 // 1億
#define UNMARKED 0
#define MARKED 1

#define BLOCK_LOW(id, p, n)  ( (id) * (n) / (p) ) // first element controlled by process i
#define BLOCK_HIGH(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - 1) // last element controlled by process i = first element controlled by process i + 1
#define BLOCK_SIZE(id, p, n) ( BLOCK_LOW((id) + 1, p, n) - BLOCK_LOW(id, p, n) )
#define BLOCK_OWNER(index, p, n) ( ( (p) * ((index) + 1) - 1) / n )

int main(int argc, char *argv[]) {
    
    int id; // id of CPU
    int p; // number of processes
    int total_len = (N - 1) / 2; // calc odds only
    char *nums; // 2 to N

    int low; // lowest num of curr p
    int high; // highest num of curr p
    int size; // num size of curr p
    int p0_size; // num size of process 0
    int prime; // prime to broadcast
    int first; // first local index
    int index; // indexing for process 0

    int count = 0; // number of primes
    int global_count; // number of total primes

	double time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Barrier(MPI_COMM_WORLD); // stop to sync all processes

	time = -MPI_Wtime(); // start the timer

    low = 2 * BLOCK_LOW(id, p, total_len) + 3;
    high = 2 * BLOCK_HIGH(id, p, total_len) + 3;
    size = BLOCK_SIZE(id, p, total_len);

    nums = (char *)calloc(size, sizeof(char));

    if (nums == NULL) {
        printf("error while calloc\n");
        MPI_Finalize();
        exit(1);
    }

    if (id == 0) {
        index = 0;
    }

    prime = 3; // start from 3 cuz 2 is the only even

    do {
        if (prime * prime > low) {
            first = ((prime * prime) - low) / 2; // start from kk
        }
        else {
            int remain = low % prime;
            if (remain == 0) {
                first = 0;
            }
            else {
                int diff = prime - remain;
                if ((low + diff) % 2 == 0) { // if is even
                    diff += prime; // add prime so its odd
                }
                first = diff / 2;
            }
        }

        for (int i = first; i < size; i += prime) {
            nums[i] = MARKED;
        }

        if (id == 0) {
            while (nums[++index] == MARKED); // finding till it's unmarked
            prime = 2 * index + 3; // index to actual num
        }

        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD); // process 0 sends new prime to all processes
    } while (prime * prime <= N);

    for (int i = 0; i < size; i++) {
        if (nums[i] == UNMARKED) {
            count++;
        }
    }

    MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // sum the prime count
	
    time += MPI_Wtime(); // stop the timer

    // printing
    if (id == 0) {
        global_count += 1; // add 2 back
		printf("%d process, 2 to %d prime : %d primes, used %f s\n", p, N, global_count, time);
    }

    free(nums);
    MPI_Finalize();

    return 0;

}

