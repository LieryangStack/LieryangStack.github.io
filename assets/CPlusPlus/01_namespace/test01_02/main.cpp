#include "ns.h"

int
main (int argc, char **argv) {

  std::cout << "--------test1 ::global variable---------" << std::endl;
  
  ns::func ();

  std::cout << "---------test2 inline namespace --------" << std::endl;

  /* 使用 inline 命名空间，可以 Parent::foo() 直接就可以调用该函数。无需 Parent::V2::foo()  */
  Parent::foo ();

  return EXIT_SUCCESS;
}