#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char **argv)
{
  int tid;

  tid = omp_get_thread_num();
  printf("TID(%d): Before parallel\n",tid);

#pragma omp parallel private(tid)
  {
    tid = omp_get_thread_num();
    printf("TID(%d): Inside parallel\n",tid);
  }

  tid = omp_get_thread_num();
  printf("TID(%d): After parallel\n",tid);

  return 0;
}
