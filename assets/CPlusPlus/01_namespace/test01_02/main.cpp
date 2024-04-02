#include "ns.h"

int
main (int argc, char **argv) {

  std::cout << "--------test1 ::global variable---------" << std::endl;
  
  ns::func ();

  std::cout << "---------test2 inline namespace --------" << std::endl;

  Parent::foo ();

  return EXIT_SUCCESS;
}