#include <string.h>
#include <iostream>

using namespace std;

/**
 * C++中没有接口的概念
 * C++中可以使用纯虚函数实现接口
 * 接口类中只有函数原型定义，没有任何数据的定义。
 */

class Interface {
public:
  virtual void print() = 0;
};

class Parent : public Interface {
public:
  /* 如果没有实现纯虚函数，该子类也是抽象类 */
  virtual void print () {
    printf ("a = %d\n", a);
  }
  int a;
};

int 
main (int argc, char *argv[]) {

  Parent p;

  p.print();

  return EXIT_SUCCESS;
}