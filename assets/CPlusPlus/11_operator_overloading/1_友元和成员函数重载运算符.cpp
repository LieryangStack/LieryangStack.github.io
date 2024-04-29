#include <iostream>

using namespace std;

class Complex {
public:
  Complex (float x = 0, float y = 0) : _x(x), _y(y) {
    cout << "Complex (float x = 0, float y = 0) : _x(x), _y(y)" << endl; 
  }

  Complex (const Complex& c1) {
    cout << " Complex (const Complex& c1) " << endl;
    this->_x = c1._x;
    this->_y = c1._y;
  }

  ~Complex () {
    cout << "~Complex () " << endl;
  }

  friend const Complex operator+(int c1, const Complex& c2);

  friend const Complex operator+(const Complex& c1, const Complex& c2);

  const Complex operator+(const Complex& another);

private:
  float _x;
  float _y;
};

const Complex 
operator+(int c1, const Complex& c2) {
  cout << "operator+(int c1, const Complex& c2)友元函数重载" << endl;
  const Complex c3(c1 + c2._x, 0 + c2._y);
  return c3;
}

const Complex 
operator+(const Complex& c1, const Complex& c2) {
  cout << "operator+(Complex& c1, const Complex& c2)友元函数重载" << endl;
  const Complex c3(c1._x + c2._x, c2._y + c2._y);
  return c3;
}

const Complex 
Complex::operator+(const Complex& another) {
  cout << "Complex::operator+(const Complex& another) 成员函数重载" << endl;
  return Complex (this->_x + another._x, this->_y + another._y);
}

int 
main(int argc, char const *argv[]) {

  Complex c1(2, 3);
  Complex c2(3, 4);

  Complex c3 = c1 + c2;
  Complex c4 = 2 + c3;


  return EXIT_SUCCESS;
}