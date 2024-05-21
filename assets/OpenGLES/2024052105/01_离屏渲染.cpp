/* 
 * compile with:   gcc  -lX11 -lEGL -lGLESv2  main.cpp
 */
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <malloc.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

#include <glib-2.0/glib.h>

#include "stb_image.h"


/**
 * location = 0 ，其中的location可以显式地指定着色器程序中的输入和输出变量在内存布局中的位置
 *                因为着色器程序是一个相对独立的程序，我们不能通过变量名来赋值变量值，
 *                所以通过location标识内存的位置传递数据。
 * glVertexAttribPointer 中第一个参数就是location变量的值
 * glEnableVertexAttribArray 的参数也是location变量的值
*/
const char *vertexShaderSource = 
    "#version 320 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0f);\n"
    "   ourColor = aColor;\n"
    "   TexCoord = vec2(aTexCoord.x, 1.0f - aTexCoord.y);\n"
    "}\0";

/**
 * FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
 * 我们还可以把得到的纹理颜色与顶点颜色混合，只需把纹理颜色与顶点颜色在片段着色器中相乘来混合二者的颜色
*/

const char *fragmentShaderSource = 
    "#version 320 es\n"
    "#extension GL_OES_EGL_image_external_essl3: require\n"
    "precision mediump float;\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor\n;"
    "in vec2 TexCoord;\n"
    "uniform sampler2D tex;"
    "void main()\n"
    "{\n"
    "   FragColor = texture(tex, TexCoord);\n"
    "}\n\0";


int 
main(int argc, char* argv[]) {

	Display* display = NULL;		/* X Display结构体指针 */
	int screen_num;		/* 屏幕序号，X11角度来看多屏幕与现在多屏幕是不同的  */
	Window win;			/* 被创建的窗口ID */
	unsigned int display_width, 
							display_height;	/*  display显示屏的长和宽 */
	unsigned int width, height;	/* win显示窗口的长和宽  */
	unsigned int win_x, win_y;	/* win显示窗口的坐标*/
	unsigned int win_border_width; /* win显示窗口边界宽度 */
	char *display_name = getenv("DISPLAY");  /* 获取环境变量DISPLAY的值 */
	XEvent event;

  EGLint major = 0;
  EGLint minor = 0;
  EGLint num_configs = 0;
	EGLDisplay egl_display = NULL;
	EGLSurface egl_surface = NULL;
	EGLContext egl_context = NULL;
  EGLConfig egl_config = NULL;
  EGLConfig *configs_list = NULL;

  display = XOpenDisplay(display_name);
  if (display == NULL) {
    fprintf(stderr, "%s: cannot connect to X server '%s'\n",
            argv[0], display_name);
    exit(1);
  }

  /* 得到显示屏幕序号，一般screen_num都是0 */
  screen_num = DefaultScreen(display);
  
  /* 显示屏长宽 */
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);

  /* 新建窗口大小是显示屏大小的三分之一*/
  width = (display_width / 10);
  height = (display_height / 3);

  /* 放到屏幕左上角 */
  win_x = 0;
  win_y = 0;

  /**
   * 所有参数在头文件中都有注释，这里特别主要的是 RootWindow(display, screen_num)
  */
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            win_x, win_y, width, height, 2,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

  /* 设定接受事件类型 */
  XSelectInput(display, win, ButtonPressMask|StructureNotifyMask );

  XMapWindow(display, win); /* 缺少映射到窗口函数，则不会显示 */

  /* 函数的主要目的是将任何还未发送到X server的缓冲区中的命令强制发送出去。*/
	XFlush(display);
  
  egl_display = eglGetDisplay( (EGLNativeDisplayType) display );
  if ( egl_display == EGL_NO_DISPLAY || eglGetError() != EGL_SUCCESS ) {
    g_error ("Got no EGL display.\n");
    return FALSE;
  }
  
  if ( eglInitialize( egl_display, &major, &minor) == EGL_FALSE || eglGetError() != EGL_SUCCESS) {
    g_error ("Unable to initialize EGL\n");
    return FALSE;
  }
  g_print ("egl version = %d.%d\n", major, minor);

  /**
   * 期望的EGL帧缓存配置列表,配置为一个key一个value的形式 
   * 指定EGL surface类型
  */
  EGLint attr[] = {       // some attributes to set up our egl"nterface
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };

  if ( eglChooseConfig( egl_display, attr, &egl_config, 1, &num_configs ) == EGL_FALSE ) {
    g_error("Failed to choose config (eglError: %d)\n", eglGetError());
    return FALSE;
  }
 
  if ( num_configs != 1 ) {
    g_error("Didn't get exactly one config, but %d\n", num_configs);
    return FALSE;
  }
   
	/* 通过egl和NativeWindow以及EGL帧缓冲配置创建EGLSurface。最后一个参数为属性信息，0表示不需要属性) */
  EGLint surface_attrs[] = {
    EGL_WIDTH, 1920,
    EGL_HEIGHT, 1080,
    EGL_NONE
  };
  
	// egl_surface = eglCreatePbufferSurface ( egl_display, egl_config, surface_attrs);
  egl_surface = eglCreateWindowSurface ( egl_display, egl_config, win, NULL);
	if ( egl_surface == EGL_NO_SURFACE ) {
		fprintf (stderr, "Unable to create EGL surface (eglError: %d\n", eglGetError());
		return FALSE;
	}

	/* 用于指定请求的 OpenGL 或 OpenGL ES 上下文的主版本和此版本号 */
	EGLint ctxattr[] = { 
    EGL_CONTEXT_MAJOR_VERSION, 3, 
    EGL_CONTEXT_MINOR_VERSION, 2, 
    EGL_NONE 
  };

	/* 通过Display和上面获取到的的EGL帧缓存配置列表创建一个EGLContext， EGL_NO_CONTEXT表示不需要多个设备共享上下文 */
	egl_context = eglCreateContext ( egl_display, egl_config, EGL_NO_CONTEXT, ctxattr );
	if ( egl_context == EGL_NO_CONTEXT ) {
		g_error("Unable to create EGL context (eglError: %d\n", eglGetError());
		return FALSE;
	}

	/* 将EGLContext和当前线程以及draw和read的EGLSurface关联，关联之后，当前线程就成为了OpenGL es的渲染线程 */
	eglMakeCurrent( egl_display, egl_surface, egl_surface, egl_context );

    /* 顶点着色器 */
  /* 创建一个着色器对象，该对象通过ID来引用的 */
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

  guint VBO, VAO, EBO, FBO;

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

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  /* 先解绑 VAO，再解绑其他的，不然绘制的时候可能不会起作用 */
  glBindVertexArray(0); 
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  /* 查看该设备支持的纹理数量 */
  GLint maxTextureUnits;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits); /* maxTextureUnits = 32 */
  std::cout << "Maximum texture image units: " << maxTextureUnits << std::endl;

  // load and create a texture 
  // -------------------------  必须是 GL_TEXTURE_2D
  GLuint texture;
  glGenTextures(1, &texture);
  g_print ("texture id = %d\n", texture);
  glBindTexture(GL_TEXTURE_2D, texture); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // // 在加载图像之前设置翻转Y轴
  // stbi_set_flip_vertically_on_load(true); 已经在着色器中修改纹理坐标了
  int img_width, img_height, nrChannels;
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *img_data = stbi_load("/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenGLES/2024052105/image/one.png", \
                                  &img_width, &img_height, &nrChannels, 0);
  if (img_data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
      glGenerateMipmap(GL_TEXTURE_2D);
  } else {
      g_print ("Failed to load texture\n");
  }

  // /* 解绑纹理 */
  // glBindTexture(GL_TEXTURE_2D, 0); 

  // /* 创建帧缓冲对象 */
  // glGenFramebuffers (1, &FBO);
  // /* 绑定帧缓冲对象 */
  // glBindFramebuffer (GL_FRAMEBUFFER, FBO);
  // /* 将纹理附着到帧缓冲对象上面 */
  // glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
  // /* 解绑 */
  // glBindFramebuffer (GL_FRAMEBUFFER, 0);


  /* 使用uniform之前一定要先使用着色器程序对象 */
  glUseProgram(shaderProgram);


  /* 开启颜色混合，也就是透明通道 */
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
	XSelectInput (display, win, KeyPressMask | StructureNotifyMask);

	gboolean main_quit = FALSE;

  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

	while (!main_quit) {

		XNextEvent (display, &event);

		switch (event.type) {
			case KeyPress:
				if (event.xkey.keycode == 9) /* 9表示ESC键*/
					main_quit = TRUE;
				break;
      case ConfigureNotify: 
        width = event.xconfigure.width;
        height = event.xconfigure.height;
        glViewport(0, 0, width, height);
        
        printf("Window size changed to %d x %d\n", width, height);
        break;
      case ClientMessage:
        if ((Atom)event.xclient.data.l[0] == wmDeleteMessage) { /* 关闭窗口消息 */
          main_quit = TRUE;
        }
        break;
		}

		/* 状态设置函数 */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    /* 状态使用函数：它使用了当前状态来获取应该清除的颜色 */
    glClear(GL_COLOR_BUFFER_BIT);

    /* 用于指定当前使用的着色器程序 */
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO); 
    glBindTexture(GL_TEXTURE_2D, texture); 

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		eglSwapBuffers (egl_display, egl_surface);
	}

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  
	eglDestroyContext ( egl_display, egl_context );
	eglDestroySurface ( egl_display, egl_surface );
	eglTerminate      ( egl_display );

  XDestroyWindow    ( display, win );
	XCloseDisplay     ( display );
 
  return 0;
}

