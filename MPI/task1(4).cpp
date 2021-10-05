#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "mpi.h"

#define SIZE 32

void message(char to_send[SIZE], int rank){

	const char* msg = "Message from process ";
	strcpy(to_send, msg);
	int n = strlen(msg);
	to_send[n] = '0' + rank;
	to_send[n + 1] = '\0';
}

int main(int argc, char** argv){

	int size, rank, maxTasks = 8;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	
	char to_send[SIZE];
	char recv[maxTasks][SIZE];

	message(to_send, rank);

	for (int i = 0; i < size; ++i){
		if (i != rank){
			MPI_Send(to_send, strlen(to_send) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}	
	}
	for (int i = 0; i < size; ++i){
		if (i != rank){
			MPI_Recv(recv[i], SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			printf("process %d received: %s\n", rank, recv[i]);
		}

	}
	
	MPI_Finalize();
}
