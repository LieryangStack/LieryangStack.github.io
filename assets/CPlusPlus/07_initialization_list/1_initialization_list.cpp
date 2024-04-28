#include <iostream>

using namespace std;

class A {
public:
  A (int a) {
    this->a = a;
    cout << "a = " << a << endl;
  }
private:
  int a;
};

class A1 {
public:
  A1 (int a1) {
    this->a1 = a1;
    cout << "a1 = " << a1 << endl;
  }
private:
  int a1;
};


class B {
public:
  B (int a, int b) : a(a), b(b), c(a), a1(b) {
    cout << "b = " << b << endl;
    cout << "c = " << c << endl;
  }
private:
  const int b;
  int c;
  A1 a1;
  A a;
};

int 
main(int argc, char const *argv[]) {

  B b(10, 12);

  return EXIT_SUCCESS;
}