#ifndef __NAME_SPACE_H__
#define __NAME_SPACE_H__

#include <iostream>

namespace NsOne {
  int a = 1;
  int func () {
    std::cout << a << std::endl;
    return 0;
  }
}

namespace NsTwo {
  extern int var;
  void func ();
  class Hi;
}

namespace NsThree {
  int a = 3;
}

#endif
