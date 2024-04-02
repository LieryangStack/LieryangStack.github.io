#include "foo.h"
#include "fun.h"

using namespace std;

namespace demo {
  int c = 33;
}

int
main (int argc, char **argv) {

  cout << "a = " << demo::a << endl;
  cout << "b = " << demo::b << endl;
  cout << "c = " << demo::c << endl;

  return EXIT_SUCCESS;
}