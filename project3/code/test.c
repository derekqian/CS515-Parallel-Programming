#include "stdio.h"
#include "mpi.h"

//#define BUFSIZE 100000
#define BUFSIZE 1000

#define MPI_TAG_MERGEPIECESIZE 4
#define MPI_TAG_MERGEPIECE 5

void disp(unsigned int rank, unsigned int *buf, unsigned int size) {
  unsigned int i;
  if(size > 16) size = 16;
  printf("Process %d: dump data->\n", rank);
  for(i=0;i<size;i++) {
    printf("%08x,", buf[i]);
    if((i+1)%16 == 0) {
      printf("\n");
    }
  }
  if(i%16 != 0) {
    printf("\n");
  }
}

int main(int argc, char **argv) {  
  unsigned int i;
  unsigned int data[BUFSIZE];
  unsigned int bufsize = BUFSIZE;
  int rank, size; 
  int st_count, st_source, st_tag; 
  MPI_Status status;  

  MPI_Init(&argc, &argv);  
  MPI_Comm_size(MPI_COMM_WORLD, &size);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  printf("Process %d of %d is alive\n", rank, size); 

  for(i=0;i<2;i++) {
    if(i!=rank) {
      MPI_Send(&bufsize, 1, MPI_UNSIGNED, i, MPI_TAG_MERGEPIECESIZE, MPI_COMM_WORLD); 
      printf("Process %d: Merge piece size = %d sent to %d successfully\n", rank, bufsize, i);

      MPI_Send(data, bufsize, MPI_UNSIGNED, i, MPI_TAG_MERGEPIECE, MPI_COMM_WORLD); 
      printf("Process %d: Merge piece sent to %d successfully\n", rank, i);
      disp(rank,data,bufsize);
    }
  }

  for(i=0;i<2;i++) {
    if(i!=rank) {
      MPI_Recv(&bufsize, 1, MPI_UNSIGNED, i, MPI_TAG_MERGEPIECESIZE, MPI_COMM_WORLD, &status);  
      printf("Process %d: Merge piece size = %d received from %d\n", rank,bufsize,i); 
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */

      MPI_Recv(data, bufsize, MPI_UNSIGNED, i, MPI_TAG_MERGEPIECE, MPI_COMM_WORLD, &status);  
      printf("Process %d: Merge piece received from %d\n", rank,i); 
      disp(rank,data,bufsize);
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */
    }
  }

  MPI_Finalize(); 
  return 0; 
} 
