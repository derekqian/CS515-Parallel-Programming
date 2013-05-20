#include <stdio.h>


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

int main(int argc, char **argv) 
{
  double time_begin,time_postread,time_prewrite,time_finish;

  if(argc != 3) {
    printf("Usage: sample_sort_mpi indatafile outdatafile\n");
    return 0;
  }

  FILE *fp;
  unsigned int count;

  //    time_begin = MPI_Wtime();

    if(!(fp=fopen(argv[1],"r"))) {
      printf("Can't open indatafile %s\n", argv[1]);
      return 0;
    }
    fread(&count, sizeof(unsigned int), 1, fp);
    array = (unsigned int*)malloc(sizeof(unsigned int)*count);
    if(array == NULL) {
      printf("allocate memory failed\n");
      return 0;
    }
    if(count != fread(array, sizeof(unsigned int), count, fp)) {
      printf("Read data from file failed.\n");
      return 0;
    }
    fclose(fp);

    quicksort(0,count-1);

    //    time_prewrite = MPI_Wtime();

    if(!(fp=fopen(argv[2],"w"))) {
      printf("Process %d: Can't open outdatafile %s",rank,argv[2]);
      return 0;
    }
    fwrite(&count,sizeof(unsigned int),1,fp);
    fwrite(array,sizeof(unsigned int),count,fp);
    fclose(fp);

    //    time_finish = MPI_Wtime();

    printf("Time %4.2f\n",(time_prewrite-time_postread)*100);
    printf("Time %4.2f\n",(time_finish-time_begin)*100);

  if(array != NULL) free(array);


  return 0;
}
