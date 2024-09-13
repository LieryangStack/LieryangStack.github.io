#include <iostream>

using namespace std;

class Parent {
public:
  Parent (const char *s) {
    this->s = s;
    cout << "Parent (const char *s)" << " " << s << endl;
  }
  ~Parent() {
    cout << "~Parent()" << endl;
  }
private:
  const char *s;

};

class Child : public Parent {
public:
  /* 通过不同的函数初始化列表，选择执行不同的父类构造函数 */
  Child (int a) : Parent ("Parameter from Child!") {
    cout << "Child()" << endl;
    this->a = a;
  }
  /* 通过不同的函数初始化列表，选择执行不同的父类构造函数 */
  Child (int a, const char *s) : Parent (s) {
    cout << "Child()" << endl;
    this->a = a;
  }
  ~Child () {
    cout << "~Child()" << endl;
  }

private:
  int a;
};

int 
main (int argc, char *argv[]) {

  Child *child1 = new Child(10);
  delete child1;

  cout << "------------------" << endl;

  Child *child2 = new Child(10, "Test Str");
  delete child2;

  return EXIT_SUCCESS;
}