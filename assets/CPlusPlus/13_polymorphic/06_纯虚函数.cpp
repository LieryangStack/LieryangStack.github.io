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
  virtual void test1(int) {};
};

class Parent : public Interface {
public:
  /* 如果没有实现纯虚函数，该子类也是抽象类 */
  virtual void print () {
    printf ("a = %d\n", a);
  }

  void test1(int a, int b) override {  /* 使用override关键字，表示这个函数是要重新虚函数 */
    printf ("test a = %d\n", a);
  }

public:
  int a;
};

int 
main (int argc, char *argv[]) {

  Parent p;

  p.print();

  return EXIT_SUCCESS;
}