#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* child(void* ip)
{
  printf("Child %d\n", (*(int*)ip));
  return NULL;
}

int main()
{
  int i=1,j=2;
  pthread_t thread1,thread2;
  pthread_create(&thread1,NULL,child,(void*)&i);
  pthread_create(&thread2,NULL,child,(void*)&j);
  printf("Parent: waiting for child 1\n");
  pthread_join(thread1,NULL);
  printf("Child 1 jointed\n");
  printf("Parent: waiting for child 2\n");
  pthread_join(thread2,NULL);
  printf("Child 2 jointed\n");
  printf("Parent: done\n");
  return 0;
}
