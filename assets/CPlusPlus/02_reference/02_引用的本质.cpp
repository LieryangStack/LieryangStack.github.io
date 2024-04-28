#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

struct typeA {
  int& a;
};

struct typeB {
  int * const a; /* 引用的本质 */
};

int
main (int argc, char *argv[]) {

  int a = 10;
  int& ra = a; /* 引用的时候必须初始化 */

  /* 虽然打印的结果相同，但是底层的本质引用是占用内存空间了 */
  cout << "&a = " << &a << endl; 
  cout << "&ra = " << &ra << endl;

  /* 引用占的内存空间和常指针相同 */
  cout << "sizeof (struct typeA) = " << sizeof (struct typeA) << endl;
  cout << "sizeof (struct typeB) = " << sizeof (struct typeB) << endl;

  return 0;  
}
