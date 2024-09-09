#include <iostream>

using namespace std;

class A {
public:
	A () {
		cout << "默认构造函数" << endl;
	}

	A (int a) {
		this->num = a;
		cout << "有一个变量构造函数" << endl;
	}

	A (const A &a) {
		this->num = a.num;
		cout << "拷贝构造函数" << endl;
	}

	~A () {	}

	A& operator=(const A& p) {
		cout << "等号操作符重载" << endl;
		return *this;
	}

private:
	int num;
};


/* 作函数参数 */
void 
f (A p) {
	return;
}

/* 函数返回值 */
A
f1 () {
	A p;
	cout << "f1()中p的地址 = " << &p << endl;
	return p;
}

/* 测试构造函数 */
void 
test () {
	A a;       /* 无参数构造函数 */
	A a1(100); /* 有一个变量的构造函数 */
	A a2 = a1; /* 拷贝构造 */
	A a3(a2);  /* 拷贝构造 */
}


int 
main(int argc, char const *argv[]) {

	cout << "------------------" << endl;

	A p; /* 默认构造函数 */
	A p1 = p;  /* 拷贝构造函数 */
	cout << "p1  = " << &p1 << endl;

	A p2(100); /* 默认构造函数 */
	cout << "p2 = " << &p2 << endl;
	p2 = p; /* 等号操作符重载 */
	cout << "p2（等号操作符重载赋值之后） = " << &p2 << endl<< endl;
	
	f (p2); /* 拷贝构造函数 */
	cout << "p2（p2作函数参数之后） = " << &p2 << endl << endl;

	p2 = f1 (); /* 等号操作符重载 */
	cout << "p2（f1函数的返回值赋值给p2之后）= " << &p2 << endl << endl;

	
	A p3 = f1 (); /* 默认构造函数, f1里面的p对象转正成为p3 */
	cout << "p3（f1函数的返回值直接初始化给p3） = " << &p3 << endl << endl;

	return 0;
}
