#define STB_IMAGE_IMPLEMENTATION

#include <gtk/gtk.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


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
    "uniform samplerExternalOES tex;"
    "void main()\n"
    "{\n"
    "   FragColor = texture(tex, TexCoord);\n"
    "}\n\0";



static guint VBO, VAO, EBO;
static GLuint texture[2];
static EGLImageKHR image;

EGLConfig egl_config = NULL;
EGLDisplay egl_display = NULL;
EGLContext egl_context = NULL;

static gint wait_sync = 0;

static gpointer 
egl_thread_test_func (gpointer data) {
  
  EGLint surface_attrs[] = {
    EGL_WIDTH, 1920,
    EGL_HEIGHT, 1080,
    EGL_NONE
  };
  EGLSurface  egl_pbuffer_surface = eglCreatePbufferSurface ( egl_display, egl_config, surface_attrs);
  if ( egl_pbuffer_surface == EGL_NO_SURFACE ) {
		g_print ("Unable to create EGL surface (eglError: %d\n", eglGetError());
		return FALSE;
	}

	/* 用于指定请求的 OpenGL 或 OpenGL ES 上下文的主版本和此版本号 */
	EGLint ctxattr[] = { 
    EGL_CONTEXT_MAJOR_VERSION, 3, 
    EGL_CONTEXT_MINOR_VERSION, 2, 
    EGL_NONE 
  };

	/* 通过Display和上面获取到的的EGL帧缓存配置列表创建一个EGLContext， EGL_NO_CONTEXT表示不需要多个设备共享上下文 */
	EGLConfig egl_pbuffer_context = eglCreateContext ( egl_display, egl_config, egl_context, ctxattr );
	if ( egl_pbuffer_context == EGL_NO_CONTEXT ) {
		g_print("Unable to create EGL context (eglError: %d\n", eglGetError());
		return FALSE;
	}

	/* 将EGLContext和当前线程以及draw和read的EGLSurface关联，关联之后，当前线程就成为了OpenGL es的渲染线程 */
	if ( eglMakeCurrent( egl_display, egl_pbuffer_surface, egl_pbuffer_surface, egl_pbuffer_context ) == EGL_FALSE ) {
    g_print ("Unable make context to current (eglError: %d\n", eglGetError());
    return FALSE;
  }

  // eglMakeCurrent( egl_display, egl_surface, egl_surface, egl_context );
  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress ("eglCreateImageKHR");
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC) eglGetProcAddress ("eglDestroyImageKHR");
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress ("glEGLImageTargetTexture2DOES");

    // load and create a texture 
  // -------------------------  必须是 GL_TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, texture[0]); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0); 

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture[1]); 
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0); 

  glBindTexture(GL_TEXTURE_2D, texture[0]); 

  // // 在加载图像之前设置翻转Y轴
  // stbi_set_flip_vertically_on_load(true); 已经在着色器中修改纹理坐标了
  int img_width, img_height, nrChannels;
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *img_data = stbi_load("image/test.jpg", \
                                  &img_width, &img_height, &nrChannels, 0);
  if (img_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
      glGenerateMipmap(GL_TEXTURE_2D);
  } else {
      g_print ("Failed to load texture\n");
  }

  glBindTexture(GL_TEXTURE_2D, 0); 

  const EGLint imageAttributes[] =
  {
      EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
      EGL_NONE
  };

  /**
   * EGL_NATIVE_PIXMAP_KHR  像素图创建EGLImageKHR
   * EGL_LINUX_DMA_BUF_EXT  DMA缓冲区创建EGLImageKHR
   * EGL_GL_TEXTURE_2D_KHR  使用另一个纹理创建EGLImageKHR
   * 
  */
  EGLImageKHR image = eglCreateImageKHR (egl_display, egl_pbuffer_context, EGL_GL_TEXTURE_2D_KHR,  (EGLClientBuffer)(uintptr_t)texture[0], imageAttributes);
  if (image == EGL_NO_IMAGE_KHR) {
    g_print ("EGLImageKHR Error id = 0x%X \n", eglGetError());
    
    return 0;
  }

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture[1]); 

  /**
   * GL_TEXTURE_EXTERNAL_OES
   * GL_TEXTURE_2D
  */
  glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, image);

  eglDestroyImageKHR (egl_display, image);

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0); 

  glFinish ();
  // glFlush ();

  /* 这个必须要有，我也不明白为什么，好像有 glFinish 或者 glFlush 就可以了 */
  // eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  
  g_print ("finish\n");

  g_atomic_int_inc (&wait_sync);

  GMainContext *main_context = g_main_context_new ();
  GMainLoop *main_loop = g_main_loop_new (main_context, TRUE);
  g_main_context_unref (main_context);

  g_main_loop_run (main_loop);

  g_main_loop_unref (main_loop);

  return 0;
}

static guint shaderProgram;

static void
realize (GtkWidget *widget) {
  
  gtk_gl_area_make_current (GTK_GL_AREA (widget));

  if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
    return;

  GdkGLContext *context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
  GdkDisplay *display = gdk_gl_context_get_display (context);
  egl_display = gdk_display_get_egl_display_private (display);
  egl_config = gdk_display_get_egl_config_private (display);
  egl_context = gdk_gl_context_get_egl_context_private (context);
                           
  if (gdk_gl_context_get_use_es (context)) {

    guint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      g_print ("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
    }

    guint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      g_print ("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n", infoLog);
    }

    shaderProgram = glCreateProgram();
    /* 把之前编译的着色器附加到程序对象上 */
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    /* 链接已经被附加的着色器 */
    glLinkProgram(shaderProgram);
    /* 检查是否成功 */
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      g_print ("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n", infoLog);
    }
    /* 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了 */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

    // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // colors           // texture coords
       0.95f,  0.95f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
       0.95f, -0.95f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.95f, -0.95f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.95f,  0.95f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
  };
  unsigned int indices[] = {  
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

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


  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

  glGenTextures(2, texture);
  g_print ("texture[0] = %d\n", texture[0]);
  g_print ("texture[1] = %d\n", texture[1]);

  GThread *egl_thread = g_thread_try_new ("test.egl.textrue", egl_thread_test_func, egl_context, NULL);

  while (!g_atomic_int_compare_and_exchange (&wait_sync, 1, 0));
}

static void
unrealize (GtkWidget *widget) {

}

static gboolean
render (GtkGLArea    *area,
        GdkGLContext *context) {
  
  gint major, minor;
  gdk_gl_context_get_version (context, &major, &minor);
  g_print ("version = %d\n", major * 100 + minor * 10); 

  /* 状态设置函数 */
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  /* 状态使用函数：它使用了当前状态来获取应该清除的颜色 */
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture (GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture[1]); 

  glUseProgram(shaderProgram);
  glBindVertexArray(VAO); 

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDrawArrays (GL_TRIANGLES, 0, 3);

  return TRUE;
}

static void
build_ui (GtkApplication *app) {
  GtkWidget *window;
  GtkWidget *gl_area;

  if (gtk_application_get_windows (app) != NULL)
    return;

  window = gtk_application_window_new (app);
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);

  gl_area = gtk_gl_area_new ();

  gtk_gl_area_set_allowed_apis (GTK_GL_AREA (gl_area), GDK_GL_API_GLES);

  g_print ("%d\n", gtk_gl_area_get_allowed_apis (GTK_GL_AREA(gl_area)));

  g_signal_connect (gl_area, "realize", G_CALLBACK (realize), NULL);
  g_signal_connect (gl_area, "unrealize", G_CALLBACK (unrealize), NULL);
  /* The main "draw" call for GtkGLArea */
  g_signal_connect (gl_area, "render", G_CALLBACK (render), NULL);

  gtk_window_set_child (GTK_WINDOW (window), gl_area);
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int argc, char *argv[])
{
  g_autoptr (GtkApplication) app = NULL;

  app = gtk_application_new ("org.egl", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (build_ui), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}
