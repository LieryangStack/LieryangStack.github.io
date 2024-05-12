/* Created by exoticorn ( http://talk.maemo.org/showthread.php?t=37356 )
 * edited and commented by André Bergner [endboss]
 *
 * libraries needed:   libx11-dev, libgles2-dev
 *
 * compile with:   gcc  -lX11 -lEGL -lGLESv2  main.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <glib.h>



const char vertex_src [] =
"                                        \
   attribute vec4        position;       \
   varying mediump vec2  pos;            \
   uniform vec4          offset;         \
                                         \
   void main()                           \
   {                                     \
      gl_Position = position + offset;   \
      pos = position.xy;                 \
   }                                     \
";
 
 
const char fragment_src [] =
"                                                      \
   varying mediump vec2    pos;                        \
   uniform mediump float   phase;                      \
                                                       \
   void  main()                                        \
   {                                                   \
      gl_FragColor  =  vec4( 1., 0.9, 0.7, 1.0 ) *     \
        cos( 30.*sqrt(pos.x*pos.x + 1.5*pos.y*pos.y)   \
             + atan(pos.y,pos.x) - phase );            \
   }                                                   \
";
//  some more formulas to play with...
//      cos( 20.*(pos.x*pos.x + pos.y*pos.y) - phase );
//      cos( 20.*sqrt(pos.x*pos.x + pos.y*pos.y) + atan(pos.y,pos.x) - phase );
//      cos( 30.*sqrt(pos.x*pos.x + 1.5*pos.y*pos.y - 1.8*pos.x*pos.y*pos.y)
//            + atan(pos.y,pos.x) - phase );
 
 
void
print_shader_info_log ( GLuint  shader) {
   GLint  length;
 
   glGetShaderiv ( shader , GL_INFO_LOG_LENGTH , &length );
 
   if ( length ) {
      char* buffer  = (char *) malloc(length);
      glGetShaderInfoLog ( shader , length , NULL , buffer );
      printf ("shader info: %s",  buffer);
      free (buffer);
 
      GLint success;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
      if ( success != GL_TRUE )   exit ( 1 );
   }
}
 
 
GLuint
load_shader (const char  *shader_source,
             GLenum       type ) {

   GLuint  shader = glCreateShader( type );
 
   glShaderSource  ( shader , 1 , &shader_source , NULL );
   glCompileShader ( shader );
 
   print_shader_info_log ( shader );
 
   return shader;
}
 
 
EGLDisplay egl_display = NULL;
EGLContext egl_context = NULL;
EGLSurface egl_surface = NULL;
EGLBoolean update_pos = EGL_FALSE;

GLfloat
   norm_x    =  0.0,
   norm_y    =  0.0,
   offset_x  =  0.0,
   offset_y  =  0.0,
   p1_pos_x  =  0.0,
   p1_pos_y  =  0.0;
 
GLint
   phase_loc,
   offset_loc,
   position_loc;
 
const float vertexArray[] = {
   0.0,  0.5,  0.0,
  -0.5,  0.0,  0.0,
   0.0, -0.5,  0.0,
   0.5,  0.0,  0.0,
   0.0,  0.5,  0.0 
};
 
 
static void 
render(Display* display, Window win) {

   static float  phase = 0;
   static int    donesetup = 0;
 
   static XWindowAttributes gwa;
 
   // draw
 
   if ( !donesetup ) {
    XWindowAttributes  gwa;
    XGetWindowAttributes ( display , win , &gwa );
    glViewport ( 0 , 0 , gwa.width , gwa.height );
    glClearColor ( 0.08 , 0.06 , 0.07 , 1.);    // background color
    donesetup = 1;
   }
   glClear ( GL_COLOR_BUFFER_BIT );
 
   glUniform1f ( phase_loc , phase );  // write the value of phase to the shaders phase
   phase  =  fmodf ( phase + 0.5f , 2.f * 3.141f );    // and update the local variable
 
   if ( update_pos ) {  // if the position of the texture has changed due to user action
    GLfloat old_offset_x  =  offset_x;
    GLfloat old_offset_y  =  offset_y;

    offset_x  =  norm_x - p1_pos_x;
    offset_y  =  norm_y - p1_pos_y;

    p1_pos_x  =  norm_x;
    p1_pos_y  =  norm_y;

    offset_x  +=  old_offset_x;
    offset_y  +=  old_offset_y;

    update_pos = EGL_FALSE;
   }
 
   glUniform4f ( offset_loc  ,  offset_x , offset_y , 0.0 , 0.0 );
 
   glVertexAttribPointer ( position_loc, 3, GL_FLOAT, GL_FALSE, 0, vertexArray );
   glEnableVertexAttribArray ( position_loc );
   glDrawArrays ( GL_TRIANGLE_STRIP, 0, 5 );
 
   eglSwapBuffers ( egl_display, egl_surface );  // get the rendered buffer to the screen
}
 
 
////////////////////////////////////////////////////////////////////////////////////////////
 
static int
our_error_handler ( Display    * x_display,
                    XErrorEvent* event
                    ) {
  printf ("cought X11 error %d (looks like this is not Maemo)\n", event->error_code);
  return 0;
}
 
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
   XEvent ev;

  EGLint major = 0;
  EGLint minor = 0;
  EGLint num_configs = 0;
  EGLConfig egl_config = NULL;
  EGLConfig *configs_list = NULL;


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

  g_print ("egl version %d.%d\n", major, minor);

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
 
 
   ///////  the openGL part  ///////////////////////////////////////////////////////////////
 
   GLuint vertexShader   = load_shader ( vertex_src , GL_VERTEX_SHADER  );     // load vertex shader
   GLuint fragmentShader = load_shader ( fragment_src , GL_FRAGMENT_SHADER );  // load fragment shader
 
   GLuint shaderProgram  = glCreateProgram ();                 // create program object
   glAttachShader ( shaderProgram, vertexShader );             // and attach both...
   glAttachShader ( shaderProgram, fragmentShader );           // ... shaders to it
 
   glLinkProgram ( shaderProgram );    // link the program
   glUseProgram  ( shaderProgram );    // and select it for usage
 
   //// now get the locations (kind of handle) of the shaders variables
   position_loc  = glGetAttribLocation  ( shaderProgram , "position" );
   phase_loc     = glGetUniformLocation ( shaderProgram , "phase"    );
   offset_loc    = glGetUniformLocation ( shaderProgram , "offset"   );
   if ( position_loc < 0  ||  phase_loc < 0  ||  offset_loc < 0 ) {
      fprintf (stderr, "Unable to get uniform location\n");
      return 1;
   }
 
 
   const float
      window_width  = 800.0,
      window_height = 480.0;
 
   //// this is needed for time measuring  -->  frames per second
   struct timezone  tz;
   struct timeval  t1, t2;
   gettimeofday ( &t1 , &tz );
   int  num_frames = 0;
 
   EGLBoolean quit = EGL_FALSE;
   while ( !quit ) {    // the main loop
 
      while ( XPending ( display ) ) {   // check for events from the x-server
 
         XEvent  xev;
         XNextEvent( display, &xev );
 
         if ( xev.type == MotionNotify ) {  // if mouse has moved
//            cout << "move to: << xev.xmotion.x << "," << xev.xmotion.y << endl;
            GLfloat window_y  =  (window_height - xev.xmotion.y) - window_height / 2.0;
            norm_y            =  window_y / (window_height / 2.0);
            GLfloat window_x  =  xev.xmotion.x - window_width / 2.0;
            norm_x            =  window_x / (window_width / 2.0);
            update_pos = EGL_TRUE;
         }
 
         if ( xev.type == KeyPress )   quit = EGL_TRUE;
      }
 
      render(display, win);   // now we finally put something on the screen
 
      if ( ++num_frames % 100 == 0 ) {
         gettimeofday( &t2, &tz );
         float dt  =  t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6;
         printf ("fps: %f\n", num_frames / dt);
         num_frames = 0;
         t1 = t2;
      }
//      usleep( 1000*10 );
   }
 
 
   ////  cleaning up...
   eglDestroyContext ( egl_display, egl_context );
   eglDestroySurface ( egl_display, egl_surface );
   eglTerminate      ( egl_display );
   XDestroyWindow    ( display, win );
   XCloseDisplay     ( display );
 
   return 0;
}