#include <string.h>
#include <iostream>

using namespace std;

class A {
public:

  A() {
    p = new char[20];
    strcpy(p, "obja");
    printf ("A()\n");
  }
  virtual ~A() {
    delete [] p;
    printf ("~A()\n");
  }

private:
  char *p;
};


class B : public A {
public:
    B() {
    p = new char[20];
    strcpy(p, "objb");
    printf ("B()\n");
  }
  virtual ~B() {
    delete [] p;
    printf ("~B()\n");
  }
private:
  char *p;
};

class C : public B {
public:
  C() {
    p = new char[20];
    strcpy(p, "objc");
    printf ("C()\n");
  }
  virtual ~C() {
    delete [] p;
    printf ("~C()\n");
  }
private:
  char *p;
};

void howtodelete (A *base) {
  delete base;
}


int 
main (int argc, char *argv[]) {

  C *myC = new C;

  // delete myC; /* 直接通过子类对象释放资源 不需要写virtual */

  howtodelete (myC); /* 通过父类的指针调用释放子类的资源 */

  return EXIT_SUCCESS;
}