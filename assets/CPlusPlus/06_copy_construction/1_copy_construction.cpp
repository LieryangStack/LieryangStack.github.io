#include <iostream>

using namespace std;

class A {
public:
	A () {
		cout << "Ĭ�Ϲ��캯��" << endl;
	}

	A (int a) {
		this->num = a;
		cout << "��һ���������캯��" << endl;
	}

	A (const A &a) {
		this->num = a.num;
		cout << "�������캯��" << endl;
	}

	~A () {	}

	A& operator=(const A& p) {
		cout << "�ȺŲ���������" << endl;
		return *this;
	}

private:
	int num;
};


/* ���������� */
void 
f (A p) {
	return;
}

/* ��������ֵ */
A
f1 () {
	A p;
	cout << "f1()��p�ĵ�ַ = " << &p << endl;
	return p;
}

/* ���Թ��캯�� */
void 
test () {
	A a;       /* �޲������캯�� */
	A a1(100); /* ��һ�������Ĺ��캯�� */
	A a2 = a1; /* �������� */
	A a3(a2);  /* �������� */
}


int 
main(int argc, char const *argv[]) {

	cout << "------------------" << endl;

	A p; /* Ĭ�Ϲ��캯�� */
	A p1 = p;  /* �������캯�� */
	cout << "p1  = " << &p1 << endl;

	A p2(100); /* Ĭ�Ϲ��캯�� */
	cout << "p2 = " << &p2 << endl;
	p2 = p; /* �ȺŲ��������� */
	cout << "p2���ȺŲ��������ظ�ֵ֮�� = " << &p2 << endl<< endl;
	
	f (p2); /* �������캯�� */
	cout << "p2��p2����������֮�� = " << &p2 << endl << endl;

	p2 = f1 (); /* �ȺŲ��������� */
	cout << "p2��f1�����ķ���ֵ��ֵ��p2֮��= " << &p2 << endl << endl;

	
	A p3 = f1 (); /* Ĭ�Ϲ��캯��, f1�����p����ת����Ϊp3 */
	cout << "p3��f1�����ķ���ֱֵ�ӳ�ʼ����p3�� = " << &p3 << endl << endl;

	return 0;
}
