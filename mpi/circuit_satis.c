#include <mpi.h>
#include <stdio.h>
#define EXTRACT_BIT(n, i) ( ( n & ( 1 << i ) ) ? 1 : 0 )

int main(int argc, char *argv[]) { // the params that execution command passes in
	int i;
	int id; // id of CPU
	int p; // number of processes
	int global_solutions;
	int solutions = 0;
	double time;
	double percent;
	int check_circuit(int, int);

	MPI_Init(&argc, &argv); // must be called before any other MPI function
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Barrier(MPI_COMM_WORLD); // stop to sync all processes

	time = -MPI_Wtime(); // start the timer

	// 0000 0000 0000 0000 ~ 1111 1111 1111 1111 (2^16 combinations)
	for (i = id; i <= 65535; i+=p) { // cyclic distribution: id = 0 >> 0, 4, 8, 12 ...; id = 1 >> 1, 5, 9, 13 ...; ...
		solutions += check_circuit(id, i);
	}

	MPI_Reduce(&solutions, &global_solutions, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	time += MPI_Wtime(); // stop the timer

	if (id == 0) {
		printf("%d process : %d solutions found, used %f s\n", p, global_solutions, time);
	}

	MPI_Finalize(); // must be called before exiting

}

int check_circuit(int id, int z)
{
	int v[16];
	int i;
	
	for (i = 0; i < 16; i ++)
		v[i] = EXTRACT_BIT(z, i); // store ith bit into v[i]
	
	if 
	( 
	(v[0] || v[1]) && ( !v[1] || !v[3] ) && ( v[2] || v[3] ) &&
	(!v[3] || !v[4]) && (v[4] || !v[5]) &&
	(v[5] || !v[6] ) && (v[5] || v[6]) &&
	(v[6] || !v[15]) && (v[7] || !v[8]) &&
	(!v[7] || !v[13]) && (v[8] || v[9]) &&
	(v[8] || !v[9]) && (!v[9] || !v[10]) &&
	(v[9] || v[11]) && (v[10] || v[11]) &&
	(v[12] || v[13]) && (v[13] || !v[14]) &&
	(v[14] || v[15])
	)
	{
		printf("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
		v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8],
		v[9], v[10], v[11], v[12], v[13], v[14], v[15]
		);
		fflush(stdout);
		return 1;
	}
	fflush(stdout);
	return 0;
}

// compile: shift + ctrl + B
// run: mpiexec -n <process num> .exe