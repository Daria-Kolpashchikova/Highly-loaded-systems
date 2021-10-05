#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "mpi.h"

#define SIZE 32

void message(char to_send[SIZE], int rank){

	const char* msg = "message from process ";
	strcpy(to_send, msg);
	int n = strlen(msg);
	to_send[n] = '0' + rank;
	to_send[n + 1] = '\0';
}

int main(int argc, char** argv)
{
	int numtasks, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Status status;

	int root = (rank + 1) % numtasks;

	int proc;
	
	if (rank == 0) {
	 proc = numtasks - 1;
	} else {
	 proc = rank - 1;
	}

	char to_send[SIZE];
	message(to_send, rank);

	char recv[SIZE];

	MPI_Send(to_send, strlen(to_send) + 1, MPI_CHAR, root, 0, MPI_COMM_WORLD);
	MPI_Recv(recv, SIZE, MPI_CHAR, proc, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	
	printf("Process %d received: %s\n", rank, recv);
	MPI_Finalize();
}
