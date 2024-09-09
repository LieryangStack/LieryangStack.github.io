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

  /**
   * 使用 inline 命名空间，可以 Parent::foo() 直接就可以调用该函数。无需 Parent::V2::foo() 
   */
  inline namespace V2 {
    void foo ();
  }
}


#endif
