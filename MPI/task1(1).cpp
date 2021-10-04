#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include<ctime>
#include <iostream>
#include "mpi.h"
void procNum(){

}
int main(int argc, char** argv)
{
    int size, rank;

	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	printf("Total number of processes in the communicator: %d, current procces number: %d\n", size, rank);
	
    MPI_Finalize();
	return 0;
}
