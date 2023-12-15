#include<stdlib.h>
#include<stdio.h>

typedef int (*Func) (void);

int
foo (void)
{
  printf("ok\n");
  return 1;
}

int
main (void)
{
  int (*func)(void);

  func = foo;
  
  Func p;

  p = foo;

  p();
  func();

  return 0;
}