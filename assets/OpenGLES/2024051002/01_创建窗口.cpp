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

  EGLint egl_version_major = 0;
  EGLint egl_version_minor = 0;
  EGLint num_configs = 0;
	EGLDisplay egl_display = NULL;
	EGLSurface egl_surface = NULL;
	EGLContext egl_context = NULL;
  EGLConfig egl_config = NULL;
  EGLConfig *configs_list = NULL;

  /************************************************X11部分****************************************************/
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
  

  /************************************************EGL部分****************************************************/
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
  if ( eglInitialize( egl_display, &egl_version_major, &egl_version_minor) == EGL_FALSE || eglGetError() != EGL_SUCCESS) {
    g_error ("Unable to initialize EGL\n");
    return FALSE;
  }

  g_print ("egl version %d.%d\n", egl_version_major, egl_version_minor);

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
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
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

	XSelectInput (display, win, KeyPressMask | StructureNotifyMask);

	gboolean main_quit = FALSE;

  /**
   * @brief: XInternAtom函数是X WIndow系统中用于获取或定义一个Atom（其实就是名为原子的唯一标识符）
   * @param atom_name: 这个名称在X服务器上用于查找或创建一个对应的原子。（其实类似与GQuark，字符串与数字对应起来，方便以后直接使用）
   * @param only_if_exists: 指定一个布尔值，表示是否只返回已存在的原子。如果设置为True，函数只在原子已经存在时返回其标识符；
   *                        如果该原子不存在，则返回None。如果设置为False，则如果原子不存在，函数会创建一个新的原子并返回其标识符。
   * 
   * WM_DELETE_WINDOW协议是X Window系统中用于处理窗口关闭请求的一种标准协议。
   * 该协议允许应用程序拦截由窗口管理器发送的关闭请求事件，从而决定如何处理关闭窗口的操作。
   * 例如提示用户保存未保存的更改、执行清理工作等。
  */
  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);

  /**
   * @brief: XSetWMProtocols函数在X Window系统中用于设置窗口支持的窗口管理器（WM）协议。
   * @param protocols: 指向包含协议原子的数组。每个原子表示应用程序支持的一个协议，例如WM_DELETE_WINDOW协议表示应用程序希望接收关闭窗口的请求。
   * @param count: 因为@protocols是一个数组，所以要规定数组的大小。
  */
  XSetWMProtocols(display, win, &wmDeleteMessage, 1);

  /* 查看OpenGL ES版本 */
  const GLubyte* version = glGetString(GL_VERSION);
  printf("OpenGL ES version: %s\n", version);

  /* 查看GLSL ES版本 */
  const GLubyte* shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  printf("GLSL version: %s\n", shadingLanguageVersion);

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
		eglSwapBuffers (egl_display, egl_surface);
	}


	eglDestroyContext ( egl_display, egl_context );
	eglDestroySurface ( egl_display, egl_surface );
	eglTerminate      ( egl_display );

	XDestroyWindow    ( display, win );
	XCloseDisplay     ( display );
 
  return 0;
}