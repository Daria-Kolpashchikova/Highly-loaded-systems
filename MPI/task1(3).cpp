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

int main(int argc, char** argv){

	int size, rank, root = 0, maxTasks = 8;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

    char to_send[SIZE];
	message(to_send, rank);

	if (rank == 0)
	{
		char recv[maxTasks - 1][SIZE];
		for (int i = 0; i < size - 1; ++i)
		{
			MPI_Recv(recv[i], SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Send(to_send, strlen(to_send) + 1, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			printf("Master received: %s\n", recv[i]);
		}
	}
	else {
		char recv[SIZE];
		MPI_Send(to_send, strlen(to_send) + 1, MPI_CHAR, root, 0, MPI_COMM_WORLD);
		MPI_Recv(recv, SIZE, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}

	MPI_Finalize();
}
