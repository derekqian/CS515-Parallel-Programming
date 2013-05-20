/* File: qsort_omp.c (Derek Qian for CS415/515)
**
** Quicksort algorithm (OpenMP version).
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#define MIN_SIZE 10 

#define VERBOSE 1
#define RETAILMSG(cond, msg) if(cond) printf msg
#ifdef DEBUG
#define DEBUGMSG(cond, msg) if(cond) printf msg
#else
#define DEBUGMSG(cond, msg) 
#endif

int *array = NULL;

/* Swap two array elements 
 */
void swap(int a, int b)
{
  if (a == b) return;
  int tmp = array[a];
  array[a] = array[b];
  array[b] = tmp;
}

/* Bubble sort for the base cases
 */
void bubble_sort(int low, int high)
{
  if (low > high) return;
  int i, j;
  for (i = low; i <= high; i++)
    for (j = i+1; j <= high; j++) 
      if (array[i] > array[j])
	swap(i, j);
}

/* Partition the array into two halves and return
 * the middle index
 */
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

/* Quicksort the array 
 */
void quicksort(int low, int high)
{
  if (high - low < MIN_SIZE) {
    bubble_sort(low, high);
    return;
  }
  DEBUGMSG(VERBOSE,("TID(qsort %d/%d): Working in parallel b,e=%d,%d\n",omp_get_thread_num(),omp_get_num_threads(),low,high));
  int middle = partition(low, high);
#pragma omp task firstprivate(low,middle)
  quicksort(low, middle-1);
#pragma omp task firstprivate(middle,high)
  quicksort(middle+1, high);
#pragma omp taskwait
}
 
/* Main routine for testing quicksort
 */
int main(int argc, char **argv)
{
  int N, i, j;
  char ans;
  
  /* check command line first */
  if (argc < 2) {
    printf ("Usage : qsort_seq <array_size>\n");
    exit(0);
  }
  if ((N=atoi(argv[1])) < 2) {
    printf ("<array_size> must be greater than 2\n");
    exit(0);
  }
  printf("qsort_omp: Sorting array with size %d\n", N);

  DEBUGMSG(VERBOSE,("TID(%d): Initializing in main thread\n",omp_get_thread_num()));

  /* init an array with values 1. N-1 */
  array = (int *) malloc(sizeof(int) * N);
#pragma omp parallel private(i,j) shared(array,N)
{
  DEBUGMSG(VERBOSE,("TID(part1 %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
#pragma omp for
  for (i=0; i<N; i++){
    DEBUGMSG(VERBOSE,("TID(part1 %d/%d): Working in parallel i=%d\n",omp_get_thread_num(),omp_get_num_threads(),i));
    array[i] = i+1;
  }
}

  /* randomly permute array elements */
  srand(time(NULL));
#pragma omp parallel private(i,j) shared(array,N)
{
  DEBUGMSG(VERBOSE,("TID(part2 %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
#pragma omp for
  for (i=0; i<N; i++) {
    DEBUGMSG(VERBOSE,("TID(part2 %d/%d): Working in parallel i=%d\n",omp_get_thread_num(),omp_get_num_threads(),i));
    j = (rand()*1./RAND_MAX)*(N-1);
    swap(i, j);
  }
}
#ifdef DEBUG
  printf("Want to see the input array-%d (y/n)? ",N);
  //scanf("%c", &ans);
  if (ans=='y') {
    for (i=0; i<N; i++)
      printf("%d, ", array[i]);
    printf("\n");
  }
#endif /* DEBUG */

#pragma omp parallel 
{
    DEBUGMSG(VERBOSE,("TID(qsort %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
#pragma omp single
{
  quicksort(0, N-1);
}
}

//#ifdef DEBUG
  /* Verify the result */
  for (i=0; i<N-1; i++) {
    if (array[i]>array[i+1]) {
      printf("Verification failed, array[%d]=%d, array[%d]=%d\n",
	     i, array[i], i+1, array[i+1]);
      return;
    }
  }
  printf("Sorting result verified!\n");
//#endif /* DEBUG */

  free(array);
}
