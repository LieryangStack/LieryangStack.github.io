#include <iostream>

using namespace std;

class B {public: int b;};

class B1 : virtual public B {private: int b1;};

class B2 : virtual public B {private: int b2;};

class C : public B1, public B2 {
  private: float d;
};


int 
main (int argc, char *argv[]) {

  C c;

  /* 指向的同一个内存地址 */

  cout << &(c.B::b) << endl;

  cout << &(c.B1::b) << endl;

  cout << &(c.B2::b) << endl;

  return EXIT_SUCCESS;
}