#include <iostream>

using namespace std;

namespace Ui {
  class Widget;
} 


class A {
public:
  A() {
    cout << "A()" << endl;
  }
  A(int a) {
    cout << "A(int a)" << "a = " << a << endl;
  }
};

class B : public A {
public:
  B(int a = 0);

  int b;
};

namespace Ui
{
  class Widget : public B {

  };
} // namespace name


/**
 * @note: 我可以通过 A(a) 指定父类运行那个构造函数
 *        如果没有 A(a)，则执行的  A() 构造函数
 */
B::B (int a) : A(a), b(10) {
  cout << "B(int a)" << "a = " << a << endl;
  cout << "b = " << b << endl << endl;
}

int 
main (int argc, char *argv[]) {

  Ui::Widget w;

  return EXIT_SUCCESS;
}
