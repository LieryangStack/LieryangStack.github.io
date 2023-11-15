#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int
str_compare(const void *s1, const void *s2)
{
  char *str1 = *(char **)s1;
  char *str2 = *(char **)s2;

  printf("str1 = %s  str2 = %s\n", str1, str2);

  size_t l1 = strlen(str1);
  size_t l2 = strlen(str2);

  if(l1 > l2)
    return 1;
  else if(l1 == l2)
    return 0;
  else 
    return -1;
}

int
main(int argc, char const *argv[])
{
  char *str_array[5] = {"da", "af", "daf", "d", "abcde"};
  qsort(str_array, 5, sizeof(char *), str_compare);

  for(int i = 0; i < 5; i++)
    printf("%s ", str_array[i]);
  
  printf("\n");

  return 0;
}
