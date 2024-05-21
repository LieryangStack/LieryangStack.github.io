#include <gtk/gtk.h>
#include <GLES3/gl32.h>
#include <EGL/egl.h>

const char *vertexShaderSource = 
    "#version 320 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = 
    "#version 320 es\n"
    "precision mediump float;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "   fragColor = vec4(1.0f, 0.5f, 0.8f, 1.0f);\n"
    "}\n\0";

float vertices[] = {
      /* 第一个三角形 */
    -0.5f, -0.5f, 0.0f, // left  
    0.5f, -0.5f, 0.0f, // right 
    0.0f,  0.5f, 0.0f // top
};

static guint shaderProgram;

static void
realize (GtkWidget *widget) {
  
  gtk_gl_area_make_current (GTK_GL_AREA (widget));

  if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
    return;

  GdkGLContext *context = gtk_gl_area_get_context (GTK_GL_AREA (widget));

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

  glUseProgram(shaderProgram);

  gint vPosition = glGetAttribLocation(shaderProgram, "vPosition"); /* 2.0 */

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 ,vertices); 
  glEnableVertexAttribArray(0); 

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
