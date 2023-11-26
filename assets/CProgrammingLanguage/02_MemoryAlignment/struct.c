#include<stdio.h>
#include<stddef.h>

/* sizeof (struct S1) = 12 */
struct S1{
	char c1; /* 0 */
	int n; /* 4 */
	char c2; /* 8 */
};

/* sizeof (struct S2) = 8 */
struct S2{
	int n; /* 0 */
	char c1; /* 4 */
	char c2; /* 5 */
};

struct s3 {
  double s1; /* 0 */
  char c1; /* 8 */
  int a1; /* 12 */
  int a2; /* 16 */
  int a3;
};

 
int main() {
	struct S1 s1;
	struct S2 s2;

	printf("%ld\n",sizeof(s1));
	printf("%ld\n",sizeof(s2));

  printf("sizeof(struct s3) = %ld\n",sizeof(struct s3));
  printf("offsetof(struct s3, s1) = %ld\n",offsetof(struct s3, s1));
  printf("offsetof(struct s3, c1) = %ld\n",offsetof(struct s3, c1));
  printf("offsetof(struct s3, a1) = %ld\n",offsetof(struct s3, a1));
  printf("offsetof(struct s3, a2) = %ld\n",offsetof(struct s3, a2));

	return 0;
}