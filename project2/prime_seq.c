/* File: prime_seq.c (for CS415/515)
**
** Eratosthenes' prime-finding algorithm (Sequential version).
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

  array = (int *) malloc(sizeof(int)*(N+1));
  for (i=2; i<=N; i++)
    array[i] = 1;

  limit = (int) sqrt((double) N);
  for (i=2; i<=limit; i++) {
    if (array[i]==1) {
      for (j=i+i; j<=N; j+=i) 
	array[j] = 0;
    }
  }

  cnt = 0;
  for (i=2; i<=N; i++) {
    if (array[i]==1)
      cnt++;
  }

#ifdef DEBUG
  printf("Total %d primes found, print all (y/n)? ", cnt);
  scanf("%c", &ans);
  if (ans=='y') {
    for (i=2; i<=N; i++) {
      if (array[i]==1)
	printf("%d, ", i);
    }
    printf("\n");
  }
#endif /* DEBUG */
}

