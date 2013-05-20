#include "stdio.h"
#include "mpi.h"

#define BUFSIZE 64

int main(int argc, char **argv) {  
  int data[BUFSIZE];  
  int rank, size; 
  int st_count, st_source, st_tag; 
  MPI_Status status;  

  MPI_Init(&argc, &argv);  
  MPI_Comm_size(MPI_COMM_WORLD, &size);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  printf("Process %d of %d is alive\n", rank, size); 
  if (rank == 0) { 
    MPI_Send(data, BUFSIZE, MPI_INT, 1, 11, MPI_COMM_WORLD); 
    printf("rank %d sent message\n", rank);
  } else if (rank == 1) { 
    MPI_Recv(data, BUFSIZE, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);  
    printf("rank %d received message\n", rank); 
    MPI_Get_count(&status, MPI_INT, &st_count); /* A temporary fix */
    st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
    st_tag = status.MPI_TAG;                    /* problem.        */
  }   
  MPI_Finalize(); 
  return 0; 
} 

