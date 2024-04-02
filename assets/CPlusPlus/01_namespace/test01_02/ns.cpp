#include "ns.h"

int value = 42; /* 这个变量位于全局命名空间 */

int ns::value = 0; /* 这个变量位于Example命名空间 */

void 
ns::func () {
  int value = 24; /* 局部变量 */

  std::cout << value << std::endl; /* 输出24， 访问局部变量 */
  std::cout << ns::value << std::endl; /* 输出0， 访问ns命名空间中的变量 */
  std::cout << ::value << std::endl; /* 输出42，访问全局命名空间中的变量 */
}

void
Parent::V1::foo () {
  std::cout << "foo v1.0" << std::endl; 
}

void
Parent::V2::foo () {
  std::cout << "foo v2.0" << std::endl; 
}
