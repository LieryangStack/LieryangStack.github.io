#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>


int 
main(int argc, char const *argv[]) {

  /* 新建一个向量变量 vec，该向量齐次坐标设定为 1.0 */
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
  /* 初始化4x4单位矩阵 */
  glm::mat4 trans = glm::mat4(1.0f);

  /* 通过单位矩阵，创建位移变化矩阵 */
  trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));

  /* 矩阵相乘，此时称号操作符已经重载 */
  vec = trans * vec;

  printf ("(%0.2f, %0.2f, %0.2f)\n", vec.x, vec.y, vec.z);

  return 0;
}
