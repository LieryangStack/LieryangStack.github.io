#include <iostream>

using namespace std;

class Complex {
public:
  Complex (float x = 0, float y = 0) : _x(x), _y(y) {}

  Complex (const Complex& c1) {
    this->_x = c1._x;
    this->_y = c1._y;
  }

  ~Complex () {}

  /* 前加加 */
  friend Complex& operator++(Complex& c);
  /* 后加加 */
  friend Complex operator++(Complex& c, int);
  /* 输出 */
  friend ostream& operator<<(ostream& os, const Complex& c);
  /* 输入 */
  friend istream& operator>>(istream& is, Complex& c);
private:
  float _x;
  float _y;
};


Complex&
operator++(Complex& c) {
  c._x++;
  c._y++;
  return c;
}

Complex
operator++(Complex &c, int) {
  Complex t(c._x, c._y);/* 先返回不变的 */
  /* 原始传入的要变 */
  c._x++;
  c._y++;
  return t;
}

ostream& 
operator<<(ostream& os, const Complex& c) {
  os << "(" << c._x << "," << c._y << ")";
  return os;
}

istream& 
operator>>(istream& is, Complex& c) {
  is >> c._x >> c._y;
  return is;
}

int 
main(int argc, char const *argv[]) {

  Complex c1(2, 3);
  
  cout << c1++ << endl;

  Complex c2 = c1++;

  cout << c1++ << endl;



  return EXIT_SUCCESS;
}