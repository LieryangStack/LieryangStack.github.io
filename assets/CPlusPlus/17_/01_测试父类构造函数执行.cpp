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
  B(int a = 0);

  int b;
};

B::B (int a) : A(a), b(10) {
  cout << "B(int a)" << "a = " << a << endl;
  cout << "b = " << b << endl; 
}

int 
main (int argc, char *argv[]) {

  B b;

  return EXIT_SUCCESS;
}
