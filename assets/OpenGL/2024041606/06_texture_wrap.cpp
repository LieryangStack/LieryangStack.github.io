/**
 * Copyright(C),2023-,Ltd.
 * File name: 
 * Author: lieryang
 * Version: 1.0
 * Date: Sep-13-2023
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

#define STB_IMAGE_IMPLEMENTATION

#include "glad/glad.h"
#include "common/stb_image.h"
#include <GLFW/glfw3.h>
#include <iostream>


/**
 * 着色器对象和着色器程序对象是两个不同的实体，它们具有不同的作用和生命周期。
 * 
 * 着色器对象是用来存储和编译着色器程序的源代码的。你可以创建多个着色器对象，
 * 每个对象都代表一个特定类型的着色器（如顶点着色器、片段着色器等）。然后，你
 * 可以将这些着色器对象附加到着色器程序对象，并使用‘glLinkProgram’函数将它们
 * 链接在一起形成一个完整的着色器程序。
 * 
 * 一旦你将着色器对象链接到着色器程序对象之后，并且通过glLinkProgram函数成功链接了着色器程序，
 * 那么着色器对象就不再需要了。着色器程序已经包含了编译后的着色器代码，并且可以被OpenGL使用。
 *
 * 删除着色器对象的目的是为了释放资源和减少内存占用。当你不再需要着色器对象时，
 * 通过调用glDeleteShader函数可以删除它们。这样，OpenGL就会释放与着色器对象相关的内存和资源。
 * 
 * 需要注意的是，删除着色器对象不会影响已经链接的着色器程序。着色器程序仍然可以正常使用，
 * 因为它已经将着色器对象中的代码复制到了自己的内部。删除着色器对象只是告诉OpenGL，你不再需要这些对象了，它可以释放相关资源。
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/**
 * location = 0 ，其中的location可以显式地指定着色器程序中的输入和输出变量在内存布局中的位置
 *                因为着色器程序是一个相对独立的程序，我们不能通过变量名来赋值变量值，
 *                所以通过location标识内存的位置传递数据。
 * glVertexAttribPointer 中第一个参数就是location变量的值
 * glEnableVertexAttribArray 的参数也是location变量的值
*/
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";
/**
 * FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
 * 我们还可以把得到的纹理颜色与顶点颜色混合，只需把纹理颜色与顶点颜色在片段着色器中相乘来混合二者的颜色
*/
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor\n;"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;"
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void 
processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}


int 
main (int argc, char **argv) {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
  /* 通知GLFW将我们窗口的上下文设置为当前线程的主上下文 */
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  /* 顶点着色器 */
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  /* 编译着色器对象 */
  glCompileShader(vertexShader);
  /* 检查着色器对象编译是否成功 */
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  /* 片段着色器 */
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  /* 创建一个着色器程序对象 */
  unsigned int shaderProgram = glCreateProgram();
  /* 把之前编译的着色器附加到程序对象上 */
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  /* 链接已经被附加的着色器 */
  glLinkProgram(shaderProgram);
  /* 检查是否成功 */
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  /* 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了 */
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);


  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // colors           // texture coords
       0.8f,  0.8f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f,  3.0f, // top right
       0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, -2.0f, // bottom right
      -0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f,  -2.0f, -2.0f, // bottom left
      -0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f,  -2.0f,  3.0f  // top left 
  };
//   float vertices[] = {
//       // positions          // colors           // texture coords
//        0.8f,  0.8f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f,  3.0f, // top right
//        0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f,  0.0f, // bottom right
//       -0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f,  0.0f, // bottom left
//       -0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f,  3.0f  // top left 
//   };
  unsigned int indices[] = {  
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  unsigned int VBO, VAO, EBO;


  glGenVertexArrays(1, &VAO);

  glGenBuffers(1, &VBO);

  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // load and create a texture 
  // -------------------------
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // 设定纹理环绕 GL_REPEAT  GL_MIRRORED_REPEAT GL_CLAMP_TO_EDGE GL_CLAMP_TO_BORDER
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  /* 如果是 GL_CLAMP_TO_BORDER，设定超过范围颜色 */
  float borderColor[] = { 0.3f, 0.8f, 0.7f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


  // set texture filtering parameters
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
  // 在加载图像之前设置翻转Y轴
  stbi_set_flip_vertically_on_load(true);

  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *data = stbi_load("/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041606/img/dog.png", \
                                  &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  /**
   * 当你将缓冲区对象设置为0,实际上是在解除绑定当前与指定目标关联的缓冲区对象。
   * 这样可以防止后续对此目标的无意识的修改，从而保护当前绑定的VBO数据
  */
  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0); 


  // uncomment this call to draw in wireframe polygons.
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    /* 检查是否按下ESC键盘，是否需要关闭窗口 */
    processInput(window);

    /* 设定背景颜色 */
    glClearColor(0.2f, 0.3f, 0.3f, 2.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* 绑定纹理对象 */
    glBindTexture (GL_TEXTURE_2D, texture);

    /* 用于指定当前使用的着色器程序 */
    glUseProgram(shaderProgram);
    /* 我们只有一个VAO，其实不用每次都绑定，但是为了程序看起来条理 */
    glBindVertexArray(VAO); 

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

