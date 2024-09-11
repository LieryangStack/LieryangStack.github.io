#include <iostream>

using namespace std;

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

  B ();

  B(int a);

  int b;
};

/**
 * @note: 这个执行的就是 A() 构造函数
 */
B::B () {
  cout << "b = " << b << endl << endl;
}


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

  B b0; /* 执行的就是 A() 构造函数 */


  B b(20); /* 执行的是 A(int a) 构造函数 */


  return EXIT_SUCCESS;
}
