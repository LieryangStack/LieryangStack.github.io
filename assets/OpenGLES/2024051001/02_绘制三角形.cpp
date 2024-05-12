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

#include <GLES3/gl32.h>
#include <EGL/egl.h>

#include <glib.h>

#include <iostream>

/**
 * location = 0 ，其中的location可以显式地指定着色器程序中的输入和输出变量在内存布局中的位置
 *                因为着色器程序是一个相对独立的程序，我们不能通过变量名来赋值变量值，
 *                所以通过location标识内存的位置传递数据。
 * glVertexAttribPointer 中第一个参数就是location变量的值
 * glEnableVertexAttribArray 的参数也是location变量的值
*/
// const char *vertexShaderSource = 
//     "attribute vec4 vPosition;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vPosition;\n"
//     "}\0";
// const char *fragmentShaderSource = 
//     "precision mediump float;\n"
//     "void main()\n"
//     "{\n"
//     "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
//     "}\0";

const char *vertexShaderSource = "#version 320 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 320 es\n"
    "precision mediump float; //声明float型变量的精度为mediump\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 0.1f);\n"
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

  // glGetIntegerv(GL_MAJOR_VERSION, &major);
  // glGetIntegerv(GL_MINOR_VERSION, &minor);
  // g_print ("gl version = %d.%d\n", major, minor);

   /**
   * 1. display命名规范 hostname:display_number.screen_number
   *    可以这样理解：一台计算机可以有多个 display，一个 display 可以有多个屏幕。
   *    指定参数的时候：因为可以省略掉 hostname 和 screen_number，所以可以用 :0 、:1 、:2 等等
   * 2. 这个函数用来连接X服务器，Ubuntu Gnome桌面的X服务器就是 Xorg（开机的时候，显示管理器会运行Xorg）
  */
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

  /* 1. 新建窗口的边界为2个像素，但是由于窗口管理器的存在，这个边界是看不见的
   * 2. 只有命令行模式下，只启动 Xorg（X服务器），才可以看见边界像素
   */
  win_border_width = 2;

  /**
   * 所有参数在头文件中都有注释，这里特别主要的是 RootWindow(display, screen_num)
  */
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            win_x, win_y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

  /* 设定接受事件类型 */
  XSelectInput(display, win, ButtonPressMask|StructureNotifyMask );

  XMapWindow(display, win); /* 缺少映射到窗口函数，则不会显示 */

	/* 1. 强制处理之前所有发送到服务器（display）的请求，并且等待所有事件处理完成才返回。
	 * 2. discard: 是一个布尔值。如果设置为True，那么此函数还会清除所有尚未处理的事件。
	 *             如果设置为False，则不会丢弃这些事件。
	 * 3.注意：过度使用XSync可能会导致程序效率降低，因为它会中断程序和服务器之间的通信流，
	 *   并强制服务器处理所有事件。因此，除非有特定的理由，否则最好避免频繁调用此函数。		
	 */
  //XSync(display, False); /* 缺少同步函数则不会显示窗口 */

  /**
   * 函数的主要目的是将任何还未发送到X server的缓冲区中的命令强制发送出去。
   * 这意味着，如果有任何挂起的请求（例如创建窗口、绘图操作等），
   * 它们会被立即发送到X server，但不会等待这些命令被执行。
  */
	XFlush(display);
  
  /**
   * @brief: 获取本地显示器native_display的EGL显示连接
   * @param display_id: 指定要连接的显示器。EGL_DEFAULT_DISPLAY表示默认显示器
   * @return: 成功返回 EGLDisplay 对象，失败返回 EGL_NO_DISPLAY
  */
  egl_display = eglGetDisplay( (EGLNativeDisplayType) display );
  if ( egl_display == EGL_NO_DISPLAY || eglGetError() != EGL_SUCCESS ) {
    g_error ("Got no EGL display.\n");
    return FALSE;
  }
  
  /**
   * @brief: 初始化使用eglGetDisplay获取的EGL显示连接。对已经初始化的EGL显示连接进行初始化除了返回版本号外没有任何效果
   * @param dpy: 指定要初始化的EGL显示连接
   * @param major: 返回EGL实现的主要版本号。可以为NULL
   * @param minor: 返回EGL实现的次要版本号。可以为NULLs
   * @note: 使用eglTerminate释放与EGL显示连接关联的资源
   * @return: 如果eglInitialize失败，则返回EGL_FALSE，否则返回EGL_TRUE。当返回EGL_FALSE时，major和minor不会被修改。
  */
  if ( eglInitialize( egl_display, &major, &minor) == EGL_FALSE || eglGetError() != EGL_SUCCESS) {
    g_error ("Unable to initialize EGL\n");
    return FALSE;
  }

  g_print ("egl version = %d.%d\n", major, minor);

  /**
   * @brief: 获取系统可用的EGL配置信息
   * @param dpy: 指定要获取配置的EGL显示连接
   * @param configs: 输出参数，包含当前系统上所有有效的 EGLConfig 配置列表
   * @param config_size: 要获取多少个有效 EGLConfig 配置列表
   * @param num_config：实际返回的 GLConfig 个数
   * @return: 成功返回 TRUE，失败返回 FALSE
   * 
  */
  if ( eglGetConfigs (egl_display, NULL, 0, &num_configs) == EGL_FALSE || eglGetError() != EGL_SUCCESS) {
    g_error ("Unable to get EGL configs\n");
    return FALSE;
  }

  g_print ("egl num_configs = %d\n", num_configs);
  
  /**
   * 期望的EGL帧缓存配置列表,配置为一个key一个value的形式 
   * 指定EGL surface类型
  */
  EGLint attr[] = {       // some attributes to set up our egl-interface
    EGL_BUFFER_SIZE, 16,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };

  /**
   * @brief: 返回一个和期望的EGL帧缓存配置列表configSpec匹配的EGL帧缓存配置列表，存储在eglConfig中
   * @param attrib_list: 期望的配置属性
   * @param configs(out)： frame buffer配置列表
   * @param config_size: 想要选中符合的frame buffer配置的最大个数
   * @param num_config(out): 获取到多少个可用frame buffer配置列表
  */
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

	/* 渲染上下文EGLContext关联的帧缓冲配置列表，EGL_CONTEXT_CLIENT_VERSION表示这里是配置EGLContext的版本 */
	EGLint ctxattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

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

  /** @brief: 着色器源码附加到着色器对象上 
   *  @param shader: 这是一个着色器对象的标识符，可以是顶点着色器或片段着色器对象标识符
   *  @param count:  这是一个整数值，指定了源代码字符串数组的元素数量。
   *                 通常，这个值为1，表示只有一个源代码字符串。
   *                 如果你有多个源代码字符串组成的数组，那么这个值应该是源代码字符串数组的元素数量
   *  @param string: 这是一个指向源代码字符串的指针数组。每个源代码字符串都是一个字符数组，包含了着色器程序的源代码。
   *  @param length: 这是一个指向整数数组的指针，用于指定每个源代码字符串的长度。如果你的源代码字符串中包含了null终止符（\0），
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
       /* 第一个三角形 */
      -0.5f, -0.5f, 0.0f, // left  
       0.5f, -0.5f, 0.0f, // right 
       0.0f,  0.5f, 0.0f, // top
       /* 第二个三角形 */
       0.7f,  0.7f, 0.0f,
       0.8f,  0.7f, 0.0f,
       0.75f, 0.8f, 0.0f
  };

  const GLfloat triangleVertices[] = {
        0.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
  };

  unsigned int VBO, VAO;

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
   * @param  arrays： 这是一个指向无符号整数数组的指针，用于接收生成的VAO对象的标识符。
  */
  glGenVertexArrays(1, &VAO);

  /**
   * @brief: 是一个用于生成缓冲对象的OpenGL函数。
   *         缓冲对象用于存储和管理各种类型的数据，如顶点数据、索引数据、纹理数据等。
   *         VBO顶点缓冲对象，它会在GPU内存（显存）中存储大量顶点
   * @param       n： 这是一个整数值，指定要生成的缓冲对象的数量。
   * @param buffers： 这是一个指向无符号整数数组的指针，用于接收生成的缓冲对象的标识符。
  */
  glGenBuffers(1, &VBO);

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
   * @brief: 配置VBO缓冲对象，将数据传输到GPU中
   * 
   * glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数
   * GL_STATIC_DRAW ：数据不会或几乎不会改变。
   * GL_DYNAMIC_DRAW：数据会被改变很多。
   * GL_STREAM_DRAW ：数据每次绘制时都会改变。
  */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /**
   * @brief: 配置顶点属性指针，使得GPU可以正确解析缓冲对象中的数据。
   * @param GLuint index: 指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)
   *                      定义了position顶点属性的位置值(Location)吗？ 它可以把顶点属性的位置值设置为0。
   *                      因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0
   * @param size:         指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
   * @param GLenum type:  指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。 
   * @param GLboolean normalized: 定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，
   *                              所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
   *                              我们把它设置为GL_FALSE。
   * @param GLsizei stride: 它告诉我们在连续的顶点属性组之间的间隔。
   *                 由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。
   *                 要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）
   *                 我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
   *                 一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，
   *                 我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
   * @param const void *pointer: 需要我们进行这个奇怪的强制类型转换。
   *                             它表示位置数据在缓冲中起始位置的偏移量(Offset)。
   *                             由于位置数据在数组的开头，所以这里是0
  */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  /* 以顶点属性位置值作为参数，启用顶点属性，使得GPU可以使用缓冲对象中的数据进行渲染。；顶点属性默认是禁用的 */
  glEnableVertexAttribArray(0);
  /**
   * 当你将缓冲区对象设置为0,实际上是在解除绑定当前与指定目标关联的缓冲区对象。
   * 这样可以防止后续对此目标的无意识的修改，从而保护当前绑定的VBO数据
  */
  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  /**
   * 之后你可以解绑 VAO，以便其他 VAO 调用不会意外地修改这个 VAO，但这种情况很少发生。
   * 修改其他 VAOs 需要调用 glBindVertexArray，所以通常我们不会在不直接必要的情况下解绑 VAOs（或 VBOs）。
  */
  glBindVertexArray(0); 



  // gint vPosition = glGetAttribLocation(shaderProgram, "vPosition"); /* 2.0 */
  
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
        int width = event.xconfigure.width;
        int height = event.xconfigure.height;
        glViewport(0, 0, width, height);
        printf("Window size changed to %d x %d\n", width, height);
		}

		/* 状态设置函数 */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    /* 状态使用函数：它使用了当前状态来获取应该清除的颜色 */
    glClear(GL_COLOR_BUFFER_BIT);

    /* 用于指定当前使用的着色器程序 */
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO); 

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 ,vertices); 
    // glEnableVertexAttribArray(0); 

    glDrawArrays (GL_TRIANGLES, 0, 6);

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