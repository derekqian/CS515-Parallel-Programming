/* File: prime_omp.c (Derek Qian for CS415/515)
**
** Eratosthenes' prime-finding algorithm (OpenMP version).
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define VERBOSE 0
#define RETAILMSG(cond, msg) if(cond) printf msg
#ifdef DEBUG
#define DEBUGMSG(cond, msg) if(cond) printf msg
#else
#define DEBUGMSG(cond, msg) 
#endif

int main(int argc, char **argv)
{
  int  *array;
  int  N, limit, cnt, i, j;
  char ans;

  /* check command line first */
  if (argc < 2) {
    printf ("Usage: prime_seq <bound>\n");
    exit(0);
  }
  if ((N=atoi(argv[1])) < 2) {
    printf ("<bound> must be greater than 1\n");
    exit(0);
  }
#ifdef DEBUG
  printf("Finding primes in range 1..%d\n", N);
#endif /* DEBUG */
  printf("prime_omp: Finding primes in range 1..%d\n", N);

  DEBUGMSG(VERBOSE,("TID(%d): Initializing in main thread\n",omp_get_thread_num()));

  array = (int *) malloc(sizeof(int)*(N+1));
#pragma omp parallel private(i) shared(array)
{
  DEBUGMSG(VERBOSE,("TID(part1 %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
#pragma omp for
  for (i=2; i<=N; i++){
    DEBUGMSG(VERBOSE,("TID(part1 %d/%d): Working in parallel i=%d\n",omp_get_thread_num(),omp_get_num_threads(),i));
    array[i] = 1;
  }
}

  limit = (int) sqrt((double) N);

#pragma omp parallel private(i,j) shared(array)
{
  DEBUGMSG(VERBOSE,("TID(part2 %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
//#pragma omp for
  for (i=2; i<=limit; i++) {
    if (array[i]==1) {
#pragma omp for
      for (j=i+i; j<=N; j+=i) {
	DEBUGMSG(VERBOSE,("TID(part2 %d/%d): Working in parallel i,j=%d,%d\n",omp_get_thread_num(),omp_get_num_threads(),i,j));
	array[j] = 0;
      }
    }
  }
}

  DEBUGMSG(VERBOSE,("TID(%d): Back to main thread\n",omp_get_thread_num()));

  cnt = 0;
#pragma omp parallel private(i) shared(array)
{
  DEBUGMSG(VERBOSE,("TID(part3 %d/%d): Working in parallel\n",omp_get_thread_num(),omp_get_num_threads()));
  #pragma omp for reduction(+:cnt)
  for (i=2; i<=N; i++) {
    DEBUGMSG(VERBOSE,("TID(part3 %d/%d): Working in parallel i=%d\n",omp_get_thread_num(),omp_get_num_threads(),i));
    if (array[i]==1)
      cnt++;
  }
}

#ifdef DEBUG
  printf("Total %d primes found, print all (y/n)? ", cnt);
  // scanf("%c", &ans);
  ans = 'n';
  if (ans=='y') {
    for (i=2; i<=32; i++) {
      if (array[i]==1)
	printf("%d, ", i);
    }
    printf("\n");
  }
#endif /* DEBUG */
  printf("prime_omp: Total %d primes found\n", cnt);

  free(array);
}
