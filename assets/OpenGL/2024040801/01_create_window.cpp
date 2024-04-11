/**
 * Copyright(C),2023-,Ltd.
 * File name: 01_hello_window.cpp
 * Author: lieryang
 * Version: 1.0
 * Date: June-1-2023
 * Description: 
 * Function List:
 *  1.
 *  2.
 * History:
 *  1.Date:
 *    Author:
 *    Modification:
 *  2.Date:
 *    Author:
 *    Modification:
 *  3....
*/

/* 获取OpenGL相关函数指针的地址 */
#include <glad.h>
/* OpenGL上下文和窗口相关函数 */
#include <GLFW/glfw3.h>

#include <iostream>

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/**
 * 我使用的是Jetson Xavier Nx，没有找到 libglfw3.so库文件，存在libglfw.so
 * g++ -o 01_Hello 01_Hello.cpp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -I./glad/include
*/


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void 
processInput(GLFWwindow *window) {
  /* 检查是否按下ESC键 */
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

/**
 * 当用户改变窗口的大小的时候，视口也应该被调整。
 * 对窗口注册一个回调函数，它会在每次窗口大小被调整的时候被调用
*/
/**
 * Function: 函数名称
 * Description: 函数功能、性能等的描述
 * Calls: 被本函数调用的函数清单
 * Called By: 调用本函数的函数清单
 * Table Accessed: 被访问的表（此项仅对于牵扯到数据库操作的程序）
 * Table Updated: 被修改的表（此项仅对于牵扯到数据库操作的程序）
 * Input: 输入参数说明，包括每个参数的作用、取值说明及参数间的关系。
 * Return: 函数返回值的作用
 * Others: 其它说明
*/
void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


int 
main(int argc, char **argv) {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  /* 告诉GLFW我们要使用的OpenGL版本是3.3 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  /* 生成glad文件的时候，已经选择了CORE_PROFILE */
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  /* 通知glfw将我们窗口的上下文设置为当前线程的主上下文 */
  glfwMakeContextCurrent(window);
  /*告诉OpenGL渲染窗口的尺寸大小*/  //err ，好像不能自己调用
  //framebuffer_size_callback (window, SCR_WIDTH, SCR_HEIGHT);
  /* 告诉glfw我们希望每当窗口调整大小的时候调用这个函数 */
  glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }    

  // render loop
  // -----------
  /**
   * 函数在我们每次循环的开始前检查一次GLFW是否被要求退出，
   * 如果是的话该函数返回true然后渲染循环便结束了，之后为我们就可以关闭应用程序了。
  */
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

    /* 状态设置函数 */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    /* 状态使用函数：它使用了当前状态来获取应该清除的颜色 */
    glClear(GL_COLOR_BUFFER_BIT);

    /**
     * 函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），
     * 它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
    */
    glfwSwapBuffers(window);
    /**
     * s函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，
     * 并调用对应的回调函数（可以通过回调方法手动设置）。
    */
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}
