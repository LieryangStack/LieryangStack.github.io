/* 
 * compile with:   gcc  -lX11 -lEGL -lGLESv2  main.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysymdef.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <glib.h>

#include <iostream>


const char *vertexShaderSource = 
    "attribute vec4 vPosition;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vPosition;\n"
    "}\0";
const char *fragmentShaderSource = 
    "precision mediump float;\n"
    "void main()\n"
    "{\n"
    "  gl_FragColor = vec4(1.0, 0.5, 0.6, 0.8);\n"
    "}\0";

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
  width = (display_width / 3);
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
    EGL_BUFFER_SIZE, 32,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
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
	egl_surface = eglCreateWindowSurface ( egl_display, egl_config, win, NULL );
	if ( egl_surface == EGL_NO_SURFACE ) {
		fprintf (stderr, "Unable to create EGL surface (eglError: %d\n", eglGetError());
		return FALSE;
	}

	/* 用于指定请求的 OpenGL 或 OpenGL ES 上下文的主版本和此版本号 */
	EGLint ctxattr[] = { 
    EGL_CONTEXT_MAJOR_VERSION, 2, 
    EGL_CONTEXT_MINOR_VERSION, 0, 
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

  float vertices[] = {
       /* 第一个三角形 */
      -0.5f, -0.5f, 0.0f, 1.0f, // left  
       0.5f, -0.5f, 0.0f, 0.0f,// right 
       0.0f,  0.5f, 0.0f, 0.9f,// top
       /* 第二个三角形 */
       0.7f,  0.7f, 0.0f, 1.0f,
       0.8f,  0.7f, 0.0f, 0.0f,
       0.75f, 0.8f, 0.0f, 0.9f
  };

  gint vPosition = glGetAttribLocation(shaderProgram, "vPosition"); /* 2.0版本 */
  
	XSelectInput (display, win, KeyPressMask | StructureNotifyMask);

	gboolean main_quit = FALSE;

  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

  /* 开启颜色混合，也就是透明通道 */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), vertices); 
    glEnableVertexAttribArray(0); 

    glDrawArrays (GL_TRIANGLES, 0, 6);

		eglSwapBuffers (egl_display, egl_surface);
	}


  glDeleteProgram(shaderProgram);
  
	eglDestroyContext ( egl_display, egl_context );
	eglDestroySurface ( egl_display, egl_surface );
	eglTerminate      ( egl_display );

	XDestroyWindow    ( display, win );
	XCloseDisplay     ( display );
 
  return 0;
}

