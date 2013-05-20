/*
 * sample sort using MPI
 */
#include <stdio.h>
#include "mpi.h"

#define DUMPDATA 1
#define VERBOSE 1
#define RETAILMSG(cond, msg) if(cond) printf msg
#ifdef DEBUG
#define DEBUGMSG(cond, msg) if(cond) printf msg
#else
#define DEBUGMSG(cond, msg) 
#endif

#define MIN_SIZE 10

unsigned int *array;

void swap(int a, int b) 
{
  if (a == b) return;
  int tmp = array[a];
  array[a] = array[b];
  array[b] = tmp;
}

void bubble_sort(int low, int high)
{
  if (low > high) return;
  int i, j;
  for (i = low; i <= high; i++)
    for (j = i+1; j <= high; j++) 
      if (array[i] > array[j])
	swap(i, j);
}

int partition(int low, int high)
{
  /* Use the lowest element as pivot */
  int pivot = array[low], middle = low, i;
 
  swap(low, high);
  for(i=low; i<high; i++) {
    if(array[i] < pivot) {
      swap(i, middle);
      middle++;
    }
  }
  swap(high, middle);
  
  return middle;
}
 
void quicksort(int low, int high)
{
  if (high - low < MIN_SIZE) {
    bubble_sort(low, high);
    return;
  }
  int middle = partition(low, high);
  quicksort(low, middle-1);
  quicksort(middle+1, high);
}

unsigned int* mergesort(unsigned int* data1, unsigned int size1, unsigned int* data2, unsigned int size2)
{
  unsigned int i=0,j=0,k=0;
  unsigned int* data = (unsigned int*)malloc(sizeof(unsigned int)*(size1+size2));
  while(j<size1 && k<size2) {
    if(data1[j]<data2[k]) {
      data[i++] = data1[j++];
    } else {
      data[i++] = data2[k++];
    }
  }
  while(j<size1) {
    data[i++] = data1[j++];
  }
  while(k<size2) {
    data[i++] = data2[k++];
  }
  return data;
}

void disp(unsigned int rank, unsigned int *buf, unsigned int size) {
  unsigned int i;
  //if(size > 16) size = 16;
  RETAILMSG(DUMPDATA, ("Process %d: dump data->\n", rank));
  for(i=0;i<size;i++) {
    RETAILMSG(DUMPDATA, ("%08x,", buf[i]));
    if((i+1)%16 == 0) {
      RETAILMSG(DUMPDATA, ("\n"));
    }
  }
  if(i%16 != 0) {
    RETAILMSG(DUMPDATA, ("\n"));
  }
}

#define MPI_TAG_PACKETSIZE 0
#define MPI_TAG_PACKET 1
#define MPI_TAG_SPLITTERS 2
#define MPI_TAG_SPLITTER 3
#define MPI_TAG_MERGEPIECESIZE 4
#define MPI_TAG_MERGEPIECE 5
#define MPI_TAG_RESULTSIZE 6
#define MPI_TAG_RESULT 7

int main(int argc, char **argv) 
{
  double time_begin,time_postread,time_prewrite,time_finish;
  unsigned int i,j,k;
  unsigned int *data = NULL;
  int rank, size; 
  int st_count, st_source, st_tag; 
  MPI_Status status;  

  unsigned int segmentsize;
  unsigned int *splitter = NULL;
  unsigned int **datapiece = NULL;
  unsigned int *datasegsize = NULL;
  unsigned int *datapiecesize = NULL;

  if(argc != 3) {
    printf("Usage: sample_sort_mpi indatafile outdatafile\n");
    return 0;
  }

  MPI_Init(&argc, &argv);  
  MPI_Comm_size(MPI_COMM_WORLD, &size);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  RETAILMSG(VERBOSE, ("Process %d of %d is alive\n", rank, size)); 

  unsigned int packetsize;
  FILE *fp;
  unsigned int count;

  if (rank == 0) { 
    time_begin = MPI_Wtime();

    if(!(fp=fopen(argv[1],"r"))) {
      printf("Process %d: Can't open indatafile %s\n", rank, argv[1]);
      return 0;
    }
    fread(&count, sizeof(unsigned int), 1, fp);
    RETAILMSG(VERBOSE, ("Process %d: data size %d\n", rank, count)); 
    data = (unsigned int*)malloc(sizeof(unsigned int)*count);
    if(data == NULL) {
      printf("Process %d: allocate memory failed\n", rank);
      return 0;
    }
    if(count != fread(data, sizeof(unsigned int), count, fp)) {
      printf("Process %d: Read data from file failed.\n", rank);
      return 0;
    }
    fclose(fp);
    RETAILMSG(VERBOSE, ("Process %d: Read data from file successfully\n", rank)); 
    disp(rank,data,count);

    time_postread = MPI_Wtime();

    if(count<size*size) {
      packetsize = 0;
    } else {
      packetsize = count / size;
    }
    for(i=size-1; i>0; i--) {
      MPI_Send(&packetsize, 1, MPI_UNSIGNED, i, MPI_TAG_PACKETSIZE, MPI_COMM_WORLD); 
      RETAILMSG(VERBOSE, ("Process %d: Packetsize = %d sent to %d\n", rank, packetsize, i));
      if(packetsize != 0) {
        MPI_Send(data+count-packetsize*(size-i), packetsize, MPI_UNSIGNED, i, MPI_TAG_PACKET, MPI_COMM_WORLD); 
        RETAILMSG(VERBOSE, ("Process %d: Packet sent to %d successfully\n", rank, i));
      }
    }

    array = data;
    packetsize = count-packetsize*(size-1);
  } else { 
    MPI_Recv(&packetsize, 1, MPI_UNSIGNED, 0, MPI_TAG_PACKETSIZE, MPI_COMM_WORLD, &status);  
    RETAILMSG(VERBOSE, ("Process %d: Packetsize = %d received\n", rank, packetsize));
    MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
    st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
    st_tag = status.MPI_TAG;                    /* problem.        */
    if(packetsize != 0) {
      data = (unsigned int*)malloc(sizeof(unsigned int)*packetsize);
      if(data == NULL) {
        printf("Process %d: Allocate memory failed\n", rank);
        return 0;
      }
      MPI_Recv(data, packetsize, MPI_UNSIGNED, 0, MPI_TAG_PACKET, MPI_COMM_WORLD, &status);  
      RETAILMSG(VERBOSE,("Process %d: Packet received\n", rank)); 
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */
    }

    array = data;
  }

  // MPI_Barrier(MPI_COMM_WORLD);

  if(packetsize != 0) {
    disp(rank,array,packetsize);
    quicksort(0, packetsize-1);
    RETAILMSG(VERBOSE,("Process %d: Quick sort done\n", rank));
    disp(rank,array,packetsize);
  }

  if(packetsize==count || packetsize==0 || size==1) {
    goto WRITE_RES;
  }

  segmentsize = packetsize / size;
  if(rank == 0) {
    splitter = (unsigned int*)malloc(sizeof(unsigned int)*(size-1)*size);
  } else {
    splitter = (unsigned int*)malloc(sizeof(unsigned int)*(size-1));
  }
  if(splitter == NULL) {
    printf("Process %d: Allocate memory failed\n",rank);
    return 0;
  }
  for(i=1;i<size;i++) {
    splitter[i-1] = array[segmentsize*i];
  }

  if(rank == 0) {
    for(i=1;i<size;i++) {
      MPI_Recv(splitter+(size-1)*i, size-1, MPI_UNSIGNED, i, MPI_TAG_SPLITTERS, MPI_COMM_WORLD, &status);  
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */
      RETAILMSG(VERBOSE,("Process %d: Splitters received from %d successfully\n", rank, i));
      disp(rank,splitter,(size-1)*(i+1));
    }
    array = splitter;
    quicksort(0,(size-1)*(size)-1);
    disp(rank,splitter,(size-1)*size);
    for(i=1;i<size;i++) {
      splitter[i-1] = splitter[(size-1)*i];
    }
    disp(rank,splitter,size-1);
    for(i=1;i<size;i++) {
      MPI_Send(splitter, size-1, MPI_UNSIGNED, i, MPI_TAG_SPLITTER, MPI_COMM_WORLD); 
      RETAILMSG(VERBOSE, ("Process %d: Splitter sent to %d successfully\n", rank, i));
    }
  } else {
    MPI_Send(splitter, size-1, MPI_UNSIGNED, 0, MPI_TAG_SPLITTERS, MPI_COMM_WORLD); 
    RETAILMSG(VERBOSE, ("Process %d: Splitters sent successfully\n", rank));
    disp(rank,splitter,size-1);

    MPI_Recv(splitter, size-1, MPI_UNSIGNED, 0, MPI_TAG_SPLITTER, MPI_COMM_WORLD, &status);  
    RETAILMSG(VERBOSE,("Process %d: Splitter received\n", rank)); 
    disp(rank,splitter,size-1);
    MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
    st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
    st_tag = status.MPI_TAG;                    /* problem.        */
  }

  datapiece = (unsigned int **)malloc(sizeof(unsigned int *)*size);
  datasegsize = (unsigned int *)malloc(sizeof(unsigned int)*size);
  datapiecesize = (unsigned int *)malloc(sizeof(unsigned int)*size);
  j=0;
  for(i=0;i<packetsize && j<size-1;) {
    if(data[i]>=splitter[j]) {
      datapiecesize[j]=i;
      j++;
    } else {
      i++;
    }
  }
  while(j<size) {
    datapiecesize[j]=packetsize;
    j++;
  }
  for(i=size-1;i>0;i--) {
    datapiecesize[i]=datapiecesize[i]-datapiecesize[i-1];
  }
  RETAILMSG(VERBOSE,("Process %d: Data for each bucket done\n",rank));
  disp(rank,datapiecesize,size);

  for(i=0;i<size;i++) {
    if(i==rank) {
      j=0;
      for(k=0;k<size;k++) {
        if(i==k) {
          datapiece[k] = (unsigned int*)malloc(sizeof(unsigned int)*datapiecesize[k]);
          memcpy(datapiece[k],data+j,sizeof(unsigned int)*datapiecesize[k]);
	} else {
          MPI_Send(&(datapiecesize[k]), 1, MPI_UNSIGNED, k, MPI_TAG_MERGEPIECESIZE, MPI_COMM_WORLD); 
          RETAILMSG(VERBOSE, ("Process %d: Merge piece size = %d sent to %d successfully\n", rank, datapiecesize[k], k));

          if(datapiecesize[k] != 0) {
            MPI_Send(data+j, datapiecesize[k], MPI_UNSIGNED, k, MPI_TAG_MERGEPIECE, MPI_COMM_WORLD); 
            RETAILMSG(VERBOSE, ("Process %d: Merge piece sent to %d successfully\n", rank, k));
            disp(rank,data+j,datapiecesize[k]);
          }
	}
	j+=datapiecesize[k];
      }
    } else {
      RETAILMSG(VERBOSE,("Process %d: Receiving merge piece size from %d\n", rank,i)); 
      MPI_Recv(&(datasegsize[i]), 1, MPI_UNSIGNED, i, MPI_TAG_MERGEPIECESIZE, MPI_COMM_WORLD, &status);  
      RETAILMSG(VERBOSE,("Process %d: Merge piece size = %d received from %d\n", rank,datasegsize[i],i)); 
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */

      if(datasegsize[i] != 0) {
        datapiece[i] = (unsigned int*)malloc(sizeof(unsigned int)*datasegsize[i]);

        MPI_Recv(datapiece[i], datasegsize[i], MPI_UNSIGNED, i, MPI_TAG_MERGEPIECE, MPI_COMM_WORLD, &status);  
        RETAILMSG(VERBOSE,("Process %d: Merge piece received from %d\n", rank,i)); 
        disp(rank,datapiece[i],datasegsize[i]);
        MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
        st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
        st_tag = status.MPI_TAG;                    /* problem.        */
      }
    }
  }
  for(i=0;i<size;i++) {
    if(i!=rank) {
      datapiecesize[i] = datasegsize[i];
    }
  }

  RETAILMSG(VERBOSE,("Process %d: Data for merge\n",rank));
  for(i=0;i<size;i++) {
    disp(rank,datapiece[i],datapiecesize[i]);
  }

  for(i=0;i<size;i++) {
    if(datapiecesize[i] > 0) {
      break;
    }
    free(datapiece[i]);
  }
  unsigned int* temp2 = datapiece[i];
  unsigned int* temp1 = temp2;
  unsigned int size2 = datapiecesize[i];
  for(i++;i<size;i++) {
    if(datapiecesize[i] > 0) {
      temp2 = mergesort(temp1,size2,datapiece[i],datapiecesize[i]);
      size2 += datapiecesize[i];
      free(temp1);
      temp1=temp2;
    }
    free(datapiece[i]);
  }
  RETAILMSG(VERBOSE,("Process %d: Merge sort done\n",rank));
  disp(rank,temp2,size2);

  if(rank==0) {
    memcpy(data,temp2,sizeof(unsigned int)*size2);
    if(temp2 != NULL) free(temp2);
    temp2 = data + size2;
    for(i=1;i<size;i++) {
      MPI_Recv(&size2, 1, MPI_UNSIGNED, i, MPI_TAG_RESULTSIZE, MPI_COMM_WORLD, &status);  
      RETAILMSG(VERBOSE,("Process %d: Result size = %d received from %d\n", rank,size2,i)); 
      MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
      st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
      st_tag = status.MPI_TAG;                    /* problem.        */

      if(size2 != 0) {
        MPI_Recv(temp2, size2, MPI_UNSIGNED, i, MPI_TAG_RESULT, MPI_COMM_WORLD, &status);  
        MPI_Get_count(&status, MPI_UNSIGNED, &st_count); /* A temporary fix */
        st_source = status.MPI_SOURCE;              /* for the LAM-MPI */
        st_tag = status.MPI_TAG;                    /* problem.        */
        RETAILMSG(VERBOSE,("Process %d: Result received from %d successfully\n", rank, i));
        disp(rank,temp2,size2);
	temp2 += size2;
      }
    }
    if(temp2 != data+count) {
      printf("Process %d: Total size wrong (size = %d).\n",rank,temp2-data);
    }
    RETAILMSG(VERBOSE,("Process %d: Sort job done, size = %d\n",rank,((int)temp2-(int)data)/sizeof(unsigned int)));
  } else {
    MPI_Send(&size2, 1, MPI_UNSIGNED, 0, MPI_TAG_RESULTSIZE, MPI_COMM_WORLD); 
    RETAILMSG(VERBOSE, ("Process %d: Result size = %d sent\n", rank,size2));
    if(size2 != 0) {
      MPI_Send(temp2, size2, MPI_UNSIGNED, 0, MPI_TAG_RESULT, MPI_COMM_WORLD); 
      RETAILMSG(VERBOSE, ("Process %d: Result sent\n", rank));
    }
    if(temp2 != NULL) free(temp2);
  }

  if(datasegsize != NULL) free(datasegsize);
  if(datapiecesize != NULL) free(datapiecesize);
  if(splitter != NULL) free(splitter);

WRITE_RES:
  if(rank==0) {
    time_prewrite = MPI_Wtime();

    if(!(fp=fopen(argv[2],"w"))) {
      printf("Process %d: Can't open outdatafile %s",rank,argv[2]);
      return 0;
    }
    fwrite(&count,sizeof(unsigned int),1,fp);
    fwrite(data,sizeof(unsigned int),count,fp);
    fclose(fp);

    time_finish = MPI_Wtime();

    printf("Time %4.2f\n",(time_prewrite-time_postread)*100);
    printf("Time %4.2f\n",(time_finish-time_begin)*100);
  }

  if(data != NULL) free(data);

  // MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize(); 

  RETAILMSG(VERBOSE,("Process %d: Program end\n",rank));

  return 0;
}
