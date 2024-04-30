#include <string.h>
#include <iostream>

using namespace std;

class Parent{
public:
  Parent(int a){
    this->a = a;
  }
  virtual void func(int a){
    cout << "Parent::func(int)..." << endl;
  }
  void func(int a, int b, int c)
	{
		cout << "Parent::func(int ,int ,int )...." << endl;
	}
private:
  int a;
};


class Child : public Parent{
public:
  Child(int a, int b) :Parent(a)
	{
		this->b = b;
	}
	virtual void func(int a) 
	{
		cout << "Child: func(int)..." << endl;
	}

	void func(int a, int b) {
		cout << "Child :func(int ,int )..." << endl;
	}

	void func(int a, int b, int c)
	{
		cout << "Child ::func(int ,int ,int )..." << endl;
	}
private:
  int b;
};


void myFunc(Parent *pp){
  pp->func(10); 	//如果调用一个普通函数，编译器根本就不会查找虚函数表。
					        //只有你调用的函数，是虚函数的时候，才会去查找虚函数表

}


int main(){


  Parent *pp = new Parent(10);
  Parent *cp = new Child(100, 200);

  myFunc(pp);
  myFunc(cp);

  cout << "----" << endl;

  Parent *pp1 = new Child(10, 20);
  pp1->func(10, 20 , 30);

  delete pp;
  delete cp;

  return 0;
}