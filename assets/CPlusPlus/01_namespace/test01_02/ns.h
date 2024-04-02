#ifndef __NS_H__
#define __NS_H__

#include <iostream>

/* namespace ns命名空间用来学习域解析操作符:: */
namespace ns {
  
  extern int value;

  void func ();
}

namespace Parent {
  namespace V1 {
    void foo ();
  }

  inline namespace V2 {
    void foo ();
  }
}


#endif
