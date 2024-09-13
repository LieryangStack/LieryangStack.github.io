#include <iostream>

using namespace std;

int 
main (int argc, char *argv[]) {

  int x = 1, y = 2;
  
  /* 值传递，不会改变 x 和 y 的值 */
  auto cg = [x, y] {

    // x = 10; 不可修改，应该是const int x = main::x;
    // y = 20; 不可修改，应该是const int y = main::y;

    /* 传入的是lambda函数局部变量 x 和 y */
    printf("[cg lambda] x = %p, x = %d\n", &x, x);
    printf("[cg lambda] y = %p, y = %d\n", &y, y);
  };

    /* 值传递，不会改变 x 和 y 的值 */
  auto cg_ref = [&x, &y] {
    
    x = 10;
    y = 20;

    /* 传入的是lambda函数局部变量 x 和 y */
    printf("[cg_ref lambda] x = %p, x = %d\n", &x, x);
    printf("[cg_ref lambda] y = %p, y = %d\n", &y, y);
  };
  
  cg();

  cg_ref();

  printf("[main] x = %p, x = %d\n", &x, x);
  printf("[main] y = %p, y = %d\n", &y, y);

  return EXIT_SUCCESS;
}