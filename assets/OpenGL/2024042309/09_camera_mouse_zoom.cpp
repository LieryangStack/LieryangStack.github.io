/**
 * @brief: 建议您在组合矩阵时，先进行缩放操作，然后是旋转，最后才是位移，否则它们会（消极地）互相影响。
*/

#define STB_IMAGE_IMPLEMENTATION

#include "glad/glad.h"
#include "common/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* 摄像机所需的变量 */
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";


const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main()\n"
    "{\n"
    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(-TexCoord.x, TexCoord.y)), 0.2);\n"
    "}\n\0";


void 
processInput(GLFWwindow *window) {
  float cameraSpeed = 0.05f;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

      /* glm::normalize 表示变成单位向量 (1, 0, 0) 
       * cameraPos = (1, 0, 0) * 0.05 = (0.05, 0, 0)
       */
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    //   glm::vec3 l = glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //   printf ("l = (%0.2f, %0.2f, %0.2f)\n", l.x, l.y, l.z);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


void 
framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}


int 
main (int argc, char **argv) {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  /* 通知GLFW将我们窗口的上下文设置为当前线程的主上下文 */
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  /* 顶点着色器 */
  /* 创建一个着色器对象，该对象通过ID来引用的 */
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

  /** @brief: 着色器源码附加到着色器对象上 
   *  @param shader: 这是一个着色器对象的标识符，可以是顶点着色器或片段着色器对象标识符
   *         count:  这是一个整数值，指定了源代码字符串数组的元素数量。
   *                 通常，这个值为1，表示只有一个源代码字符串。
   *                 如果你有多个源代码字符串组成的数组，那么这个值应该是源代码字符串数组的元素数量
   *         string: 这是一个指向源代码字符串的指针数组。每个源代码字符串都是一个字符数组，包含了着色器程序的源代码。
   *         length: 这是一个指向整数数组的指针，用于指定每个源代码字符串的长度。如果你的源代码字符串中包含了null终止符（\0），
   *                 那么你可以将length参数设置为NULL，OpenGL将自动计算每个字符串的长度。
   */
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

  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

     0.0f,
  };


  glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
  };

  unsigned int VBO, VAO;

  /* 创建一个 VAO 对象 */
  glGenVertexArrays(1, &VAO);

  /* 创建一个 VBO 对象 */
  glGenBuffers(1, &VBO);

  /* 绑定 VAO 对象，之后VBO和EBO的操作都会绑定到VAO中 */
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* 位置属性 */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  /* 纹理属性 */
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


  unsigned int texture1, texture2;
  
  /* 创建一个纹理对象 GL_TEXTURE_2D 绑定到该纹理对象（为了设置该纹理对象） */
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1); 
  
  /* 设置纹理对象环绕方式 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  /* 设置纹理对象过滤方式 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); /* 加载图片的时候，沿着X轴翻转图片（图片起始点在左上角，纹理坐标起始点在左下角） */
  unsigned char *data = stbi_load("/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041605/image/container.jpg",  \
                                    &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else
    std::cout << "Failed to load texture" << std::endl;

  stbi_image_free(data);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  data = stbi_load("/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGL/2024041605/image/awesomeface.png", \
                    &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else
    std::cout << "Failed to load texture" << std::endl;

  stbi_image_free(data);

  /**
   * 当你将缓冲区对象设置为0,实际上是在解除绑定当前与指定目标关联的缓冲区对象。
   * 这样可以防止后续对此目标的无意识的修改，从而保护当前绑定的VBO数据
  */
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 


  /* 使用uniform之前一定要先使用着色器程序对象 */
  glUseProgram(shaderProgram);
  /**
   * OpenGL至少保证有16个纹理单元供你使用，也就是说你可以激活从GL_TEXTURE0到GL_TEXTRUE15。（默认从0开始GL_TEXTURE0）
   * 现在，为着色器中 "texture1" 变量指定位置值是 0，"texture2" 变量指定位置值是 1
  */
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

  glEnable(GL_DEPTH_TEST);


  while (!glfwWindowShouldClose(window)) {
    /* 检查是否按下ESC键盘，是否需要关闭窗口 */
    processInput(window);

    /* 设定背景颜色 */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    /* 用于指定当前使用的着色器程序 */
    glUseProgram(shaderProgram);
    /* 我们只有一个VAO，其实不用每次都绑定，但是为了程序看起来条理 */
    glBindVertexArray(VAO); 

    /* 创建变化矩阵 */
    glm::mat4 projection    = glm::mat4(1.0f);

    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), \
                       1, GL_FALSE, glm::value_ptr(projection));

    for (unsigned int i = 0; i < 10; i++) {

      
      glm::mat4 model         = glm::mat4(1.0f);
      glm::mat4 view          = glm::mat4(1.0f);

      /* 世界 -> 观察 
       *
       * 第一个参数：摄像机的位置
       * 第二个参数：目标位置（摄像机注视点的位置）
       * 第三个参数：上向量Y（我们计算右向量，右向量就是x轴的正方向，使用的那个上向量）
       * 
       *  摄像机Z轴方向等于 =（第一个参数 减去 第二个参数）
       */
      view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
      glm::vec3 test = cameraPos + cameraFront;
    //   printf ("cameraPos = (%0.2f, %0.2f, %0.2f)\n", cameraPos.x, cameraPos.y, cameraPos.z);
    //   printf ("test = (%0.2f, %0.2f, %0.2f)\n", test.x, test.y, test.z);
      /* 局部 -> 世界 */
      model = glm::translate(model, cubePositions[i]);
      float angle = (float)glfwGetTime() + (20.0f * i);
      model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 1.0f));
      

      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), \
                        1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), \
                       1, GL_FALSE, glm::value_ptr(view));
     
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }



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

