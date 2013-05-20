/* datagen.c ---  for CS415/515 sorting project (J. Li)
**   A simple data generation engine.
**
** Usage: datagen n --- generate n 4-byte integers (default 64)
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) 
{
  unsigned int val, count = 64;
  long i;
  int size = sizeof(unsigned int);

  if (argc > 1)
    count = atol(argv[1]);
    
  fwrite(&count, size, 1, stdout);
  srand48(time(0));
  for (i=0; i<count; i++) {
    val = mrand48();
    fwrite(&val, size, 1, stdout);
  }
}

