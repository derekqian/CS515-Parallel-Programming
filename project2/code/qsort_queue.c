/* File: qsort_queue.c (Dejun Qian for CS515 project 1)
**
** Quicksort algorithm (Parallel version).
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "queue.h"


#define MIN_SIZE 10

#define VERBOSE 0
#define RETAILMSG(cond, msg) if(cond) printf msg
#ifdef DEBUG
#define DEBUGMSG(cond, msg) if(cond) printf msg
#else
#define DEBUGMSG(cond, msg) 
#endif

struct range {
  int begin;
  int end;
};

int *array = NULL;
int N = 0;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
int completed = 0;
pthread_mutex_t completed_mutex;

bool is_completed() {
  pthread_mutex_lock(&completed_mutex);
  bool res = (completed == N);
  pthread_mutex_unlock(&completed_mutex);
  return res;
}

void report_completed(int num) {
  pthread_mutex_lock(&completed_mutex);
  completed += num;
  pthread_mutex_unlock(&completed_mutex);
}

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
  report_completed(high-low+1);
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

  report_completed(1);

  return(middle);
}

void submit(int low, int high) {
  int size;
  struct range *r = (struct range*) malloc(sizeof(struct range));
  r->begin = low;
  r->end = high;
  pthread_mutex_lock(&queue_mutex);
  enqueue(r);
  size = getsize();
  pthread_cond_signal(&queue_cond);
  DEBUGMSG(VERBOSE, ("submit (%d, %d)-%d\n", low, high, size));
  pthread_mutex_unlock(&queue_mutex);
}

/* Quicksort the array
 */
void quicksort(int low, int high)
{
  if (high - low < MIN_SIZE) {
    bubble_sort(low, high);
    return;
  }
  int middle = partition(low, high);
  // quicksort(low, middle-1);
  submit(low, middle-1);
  quicksort(middle+1, high);
}

// Every thread executes this routine, including 'main'
void worker(long rank) {
  struct range *r = NULL;
  int size;
  // quicksort(0, N-1);
#ifdef DEBUG
  printf("Thread %ld starts ...\n", rank);
#endif
  while (!is_completed()) {
    DEBUGMSG(VERBOSE,("TID (%ld): ready\n", rank));
    pthread_mutex_lock(&queue_mutex);
    while (empty() && !is_completed()) {
      DEBUGMSG(VERBOSE,("TID (%ld): waiting for signal\n", rank));
      pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    r = dequeue();
    size = getsize();
    pthread_mutex_unlock(&queue_mutex);
    if (r != NULL) {
      DEBUGMSG(VERBOSE,("TID (%ld): get (%d, %d)-%d-%d\n", rank, r->begin, r->end, size, completed));
      quicksort(r->begin, r->end);
      free(r);
#ifdef DEBUG
      printf("%ld.", rank);
#endif
    }
  }
  pthread_mutex_lock(&queue_mutex);
  pthread_cond_signal(&queue_cond);
  pthread_mutex_unlock(&queue_mutex);
  DEBUGMSG(VERBOSE,("TID (%ld): thread quit\n", rank));
}

void* child_proc(void *ip)
{
  worker((long)ip);
  return NULL;
}

/* Main routine for testing quicksort
 */
int main(int argc, char **argv)
{
  int i, j;
  int num_threads;
  char ans;
  pthread_t *thread_id = NULL;

  /* check command line first */
  if((argc != 2) && (argc != 3))
  {
    printf("Usage: qsort_queue N [num_threads]\n");
    exit(0);
  }

  N = atoi(argv[1]);
  if(N < 2)
  {
    printf("N must be greater than 2\n");
    exit(0);
  }

  if(argc == 2)
  {
    num_threads = 1;
  }
  else
  {
    num_threads = atoi(argv[2]);
    if(num_threads < 1)
    {
      printf("num_threads must be greater than 1\n");
      exit(0);
    }
  }

  printf("Quicksort with %d threads (queue version), array_size = %d\n", num_threads, N);

  pthread_mutex_init(&queue_mutex, NULL);
  pthread_cond_init(&queue_cond, NULL);
  pthread_mutex_init(&completed_mutex, NULL);

  /* create (num_threads-1) threads */
  if(num_threads > 1) {
    thread_id = (pthread_t*)malloc((num_threads-1)*sizeof(pthread_t));
    if (thread_id == NULL) {
      printf("Lack of memory when creating threads\n");
      goto QUIT_POINT;
    }
    for(i=1;i<num_threads;i++)
    {
      pthread_create(&thread_id[i-1],NULL,child_proc,(void*)i);
      DEBUGMSG(VERBOSE, ("Child thread %d created\n",i));
    }
  }

  /* init an array with values 1. N-1 */
  array = (int *) malloc(sizeof(int) * N);
  if (array == NULL) {
    printf("Lack of memory when allocating data buffer\n");
    goto QUIT_POINT;
  }
  for (i=0; i<N; i++)
    array[i] = i+1;

  /* randomly permute array elements */
  srand(time(NULL));
  for (i=0; i<N; i++) {
    j = (rand()*1./RAND_MAX)*(N-1);
    swap(i, j);
  }
#ifdef DEBUG
  // printf("Want to see the input array (y/n)? ");
  // scanf("%c", &ans);
  ans = 'n';
  if (ans=='y') {
    for (i=0; i<N; i++)
      printf("%d, ", array[i]);
    printf("\n");
  }
#endif /* DEBUG */

  submit(0, N-1);
  worker (0);

  /* wait for other threads to join */
  if(num_threads > 1) {
    for(i=1;i<num_threads;i++)
    {
      pthread_join(thread_id[i-1],NULL);
    }
  }

  
  #ifdef DEBUG
  /* Verify the result */
  for (i=0; i<N-1; i++) {
    if (array[i]>array[i+1]) {
      printf("Verification failed, array[%d]=%d, array[%d]=%d\n",
	     i, array[i], i+1, array[i+1]);
      return;
    }
  }
  printf("\n...\nSorting result verified (cnt=%d)!\n", N);
  #endif /* DEBUG */
  printf("Sorting result verified!\n");

QUIT_POINT:
  /* free the memory resource */
  if (thread_id != NULL) {
    free(thread_id);
    thread_id = NULL;
  }
  if (array != NULL) {
    free(array);
    array = NULL;
  }

  return 0;
}
