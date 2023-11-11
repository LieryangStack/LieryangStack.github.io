#include "namespace.h"

/**
 * 1.命名空间内声明，外部实现
 * 2.两个相同名字的命名空间可以合并内部变量
 * 3.类的前向声明
 */

int NsTwo::var = 2;

class NsTwo::Hi {
public:
  /* C++11之后是允许的，在成员变量声明的同时进行初始化 */
  int a = 1;
};

void
NsTwo::func () {
  std::cout << NsTwo::var << std::endl;
}

namespace NsThree {
  extern int a;
  void func() {
    std::cout << a << std::endl;
  }
}

/**
 * 类的前向声明
 * 但要注意，前向声明只允许在指针和引用上使用，而不能实例化对象
 */
class Test;

int
main (int argc, char **argv) {

  NsOne::func();
  NsTwo::func();
  NsThree::func();

  Test *t1;

  return 0;
}

class Test {
public:
  int a;
};