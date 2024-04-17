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
 * 最终输出颜色现在是两个纹理的结合。
 * GLSL内建的mix函数需要接受两个值作为参数，并对它们根据第三个参数进行线性插值。
 * 如果第三个值是0.0，它会返回第一个输入；如果是1.0，会返回第二个输入值。
 * 0.2会返回80%的第一个输入颜色和20%的第二个输入颜色，即返回两个纹理的混合色。
*/
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor\n;"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main()\n"
    "{\n"
    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2);\n"
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

  /* ------------------------------------ */

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // colors           // texture coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
  };
  unsigned int indices[] = {  
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  unsigned int VBO, VAO, EBO;

  /**
   * VAO对象存储了以下与顶点数据相关的状态信息：
   * a.顶点缓冲对象（Vertex Buffer Object，VBO）的绑定状态，用于存储实际的顶点数据。
   * b.顶点属性指针的配置，包括顶点位置、法线、颜色等。
   * c.顶点索引缓冲对象的绑定状态（如果使用索引绘制）。
   * d.其他与顶点数据配置相关的状态，如顶点属性的启用/禁用状态、顶点属性分割和步长等。
   * 
   * 通过使用VAO，你可以将顶点数据的配置和状态信息封装在一个对象中，并且在需要时快速切换和重用这些配置。
   * 这有助于提高代码的可读性、可维护性，并且可以提供更高的渲染性能。
  */

  /**
   * @brief: glGenVertexArrays是一个用于生成顶点数组对象的OpenGL函数。（VAO是用于管理顶点数据的状态和配置）
   * @param       n： 这是一个整数值，指定要生成的VAO对象的数量
   *         arrays： 这是一个指向无符号整数数组的指针，用于接收生成的VAO对象的标识符。
  */
  glGenVertexArrays(1, &VAO);

  /**
   * @brief: 是一个用于生成缓冲对象的OpenGL函数。
   *         缓冲对象用于存储和管理各种类型的数据，如顶点数据、索引数据、纹理数据等。
   *         VBO顶点缓冲对象，它会在GPU内存（显存）中存储大量顶点
   * @param       n： 这是一个整数值，指定要生成的缓冲对象的数量。
   *        buffers： 这是一个指向无符号整数数组的指针，用于接收生成的缓冲对象的标识符。
  */
  glGenBuffers(1, &VBO);

  glGenBuffers(1, &EBO);

  /**
   * @brief: 函数将特定的VAO对象绑定到OpenGL的上下文中。
   *         一旦绑定了VAO对象，之后的顶点数据配置和操作都会与该VAO对象相关联
  */
  glBindVertexArray(VAO);

  /**
   * @brief: 将特定的缓冲对象绑定到OpenGL的上下文中。
   *         一旦绑定了缓冲对象，之后的数据操作和配置都会与该缓冲对象相关联。
  */
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
  /**
   * @brief: 配置缓冲对象，将数据传输到GPU中
   * 
   * glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数
   * GL_STATIC_DRAW ：数据不会或几乎不会改变。
   * GL_DYNAMIC_DRAW：数据会被改变很多。
   * GL_STREAM_DRAW ：数据每次绘制时都会改变。
  */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  /**
   * @brief: 配置顶点属性指针，使得GPU可以正确解析缓冲对象中的数据。
   * GLuint index: 指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)
   *               定义了position顶点属性的位置值(Location)吗？ 它可以把顶点属性的位置值设置为0。
   *               因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0
   * GLint size: 指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
   * GLenum type: 指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。 
   * GLboolean normalized: 定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，
   *                       所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
   *                       我们把它设置为GL_FALSE。
   * GLsizei stride: 它告诉我们在连续的顶点属性组之间的间隔。
   *                 由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。
   *                 要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）
   *                 我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
   *                 一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，
   *                 我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
   * const void *pointer: 需要我们进行这个奇怪的强制类型转换。
   *                      它表示位置数据在缓冲中起始位置的偏移量(Offset)。
   *                      由于位置数据在数组的开头，所以这里是0
  */
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // /* 以顶点属性位置值作为参数，启用顶点属性，使得GPU可以使用缓冲对象中的数据进行渲染。；顶点属性默认是禁用的 */
  // glEnableVertexAttribArray(1);

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
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("./image/container.jpg", &width, &height, &nrChannels, 0);
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
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("./image/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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


  /*使用uniform之前一定要先使用着色器程序对象*/
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    /* 检查是否按下ESC键盘，是否需要关闭窗口 */
    processInput(window);

    /* 设定背景颜色 */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

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

