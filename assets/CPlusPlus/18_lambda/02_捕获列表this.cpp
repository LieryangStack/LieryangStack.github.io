#include <iostream>

using namespace std;

class A {
public:

  A () {
    printf("this = %p\n", this);
    auto cg = [*this]() {  /* this是指针，*this才是值传递 */
      printf("this = %p\n", this);
      // this.a = 100;
    };
    
    auto cg_p = [this]() {  /* this是指针，this就是传递指针的值 */
      printf("this = %p\n", this);
      this->a = 100;
    };


    cg_p();

    cout << this->a << endl;
  }


public:
  int a;
};

int 
main (int argc, char *argv[]) {

  A a;

  return EXIT_SUCCESS;
}