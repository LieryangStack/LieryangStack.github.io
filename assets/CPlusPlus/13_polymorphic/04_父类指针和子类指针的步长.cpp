#include <string.h>
#include <iostream>

using namespace std;

class Parent{
public:
  Parent(int a){
    this->a = a;
  }
  virtual void print(){
    cout << "Parent::print a = " << a << endl;
  }
  int a;
};


class Child : public Parent{
public:
  Child(int a) : Parent(a){

  }
  virtual void print(){
    cout << "Child::print a = " << a << endl;
  }

  int b;
};

int main(){

  Child array[] = {Child(0), Child(1), Child(2)};

  Parent *pp = &array[0];
  Child *cp = &array[0];

  pp->print();
  cp->print();

  pp++;
  cp++;

  /*
  pp->print();
  cp->print();
  */

  int i = 0;
  for(cp = &array[0], i = 0; i < 3; i++, cp++){
    cp->print();
  }


  return 0;
}