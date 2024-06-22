#include <stdio.h>

int 
main(int argc, char **argv) {

#ifdef CRT_SECURE_NO_DEPRECATE
  printf ("hello word %s\n", CRT_SECURE_NO_DEPRECATE);
#endif

  return 0;
}