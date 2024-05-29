#define STB_IMAGE_IMPLEMENTATION

#include <gtk/gtk.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


#include "stb_image.h"

#include <gst/gst.h>

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

/* Structure to contain all our information, so we can pass it to callbacks */
/* 这里存下了所有需要的局部变量，因为本教程中会有回调函数，使用struct比较方便 */
typedef struct _CustomData
{
  GstElement *pipeline;
  GstElement *source;

  GstElement *h264depay;
  GstElement *h264parse;
  GstElement *queue;
  GstElement *decode;
  GstElement *convert;
  GstElement *filter;
  GstElement *sink;

} CustomData;

GtkWidget *window;
GtkWidget *gl_area;

/* Handler for the pad-added signal */
static void pad_added_handler (GstElement * src, GstPad * pad,
    CustomData * data);

gboolean
print_pad_structure(GQuark   field_id,
                    const GValue * value,
                    gpointer user_data){

  g_print("****foreach****\n");
  g_print("%s\n", G_VALUE_TYPE_NAME(value));
  if( g_type_is_a(value->g_type, G_TYPE_INT))
    g_print("%d, %d\n",field_id,g_value_get_int(value));
  else
    g_print("%d, %s\n",field_id,g_value_get_string(value));

  return TRUE;
}

gboolean
gst_event_callback(GstPad *pad, GstObject *parent, GstEvent *event){
  g_print("***Event\n");
  return TRUE;
}

static void
pad_added_handler (GstElement * src, GstPad * new_pad, CustomData * data)
{
  GstPad *sink_pad = NULL;
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;  
  
  new_pad_caps = gst_pad_get_current_caps (new_pad);
  /* 通过gst_caps_get_size函数可以的出Caps中有几个GstStructure */
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);

  g_message("%s type is %s\n", GST_PAD_NAME (new_pad), new_pad_type);
  // gst_structure_foreach (new_pad_struct, print_pad_structure, NULL);

  sink_pad = gst_element_get_static_pad (data->h264depay, "sink");

  // static int i = 0;
  // if (i++ == 0)
  //   sink_pad = gst_element_get_static_pad (data->h264depay, "sink");
  // else 
  //   sink_pad = gst_element_get_static_pad (data->h264depay_t, "sink");

  /* If our converter is already linked, we have nothing to do here */
  if (gst_pad_is_linked (sink_pad)) {
    g_message ("We are already linked. Ignoring.\n");
    goto exit;
  }

  /* Attempt the link */
  ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_message ("Type is '%s' but link failed. ret = %d\n", new_pad_type, ret);
  } else {
    g_message ("Link succeeded (type '%s').\n", new_pad_type);
  }

exit:
  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL)
    gst_caps_unref (new_pad_caps);

  /* Unreference the sink pad */
  gst_object_unref (sink_pad);
}


static guint VBO, VAO, EBO;
static GLuint texture[2];
static EGLImageKHR image;

static guint shaderProgram;

EGLConfig egl_config = NULL;
EGLDisplay egl_display = NULL;
EGLContext egl_context = NULL;

static gint wait_sync = 0;

static void 
ui_render_cb (GstElement *sink, gpointer user_data) {
  gtk_gl_area_queue_render (GTK_GL_AREA (gl_area));
   
  g_print ("1\n");
}

static gpointer 
egl_thread_test_func (gpointer user_data) {

  CustomData data;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  // g_setenv("GST_DEBUG_DUMP_DOT_DIR", "./", TRUE);

  /* Initialize GStreamer */
  gst_init (NULL, NULL);

  /* Create the elements */
  data.source = gst_element_factory_make ("rtspsrc", "source");

  data.h264depay = gst_element_factory_make ("rtph264depay", "rtph264depay");
  data.h264parse = gst_element_factory_make ("h264parse", "h264parse");
  data.queue = gst_element_factory_make ("queue", "queue");
  data.decode = gst_element_factory_make ("nvv4l2decoder", "nvv4l2decoder"); // nvv4l2decoder avdec_h264
  data.filter = gst_element_factory_make("capsfilter", "filter");
  data.convert = gst_element_factory_make ("nvvideoconvert", "videoconvert");

  data.sink = gst_element_factory_make ("nveglglessink", "sink"); //nv3dsink

  g_signal_connect (data.sink, "ui-render", ui_render_cb, NULL);

  GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                      "format", G_TYPE_STRING, "RGBA",
                                      NULL);
  GstCapsFeatures *feature = gst_caps_features_new ("memory:NVMM", NULL);
  gst_caps_set_features (caps, 0, feature);

  // 设置Caps到filter
  g_object_set(G_OBJECT(data.filter), "caps", caps, NULL);
  gst_caps_unref(caps);

  g_print ("UI     egl_display=%p\n", egl_display);
  g_print ("UI     egl_config=%p\n", egl_config);
  g_print ("UI     egl_context=%p\n", egl_context);

  g_object_set (data.sink, "egl-display", egl_display,
                           "egl-config", egl_config,
                           "egl-share-context", egl_context,
                           "egl-share-texture", texture[0], NULL);

  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");

  if (!data.pipeline || !data.source || !data.h264depay || !data.h264parse || !data.queue \
      || !data.decode || !data.convert || !data.filter || !data.sink) {
    g_printerr ("Not all elements could be created.\n");
    return NULL;
  }

  /* Build the pipeline. Note that we are NOT linking the source at this
   * point. We will do it later. */
  gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.h264depay, data.queue, \
      data.h264parse, data.decode, data.convert, data.filter, data.sink, NULL);


  if (!gst_element_link_many (data.h264depay, data.h264parse, data.queue, \
                              data.decode, data.convert, data.filter, data.sink, NULL)) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return NULL;
  }

  /* Set the URI to play */
  g_object_set(data.source, "location", "rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101", \
                            "latency", 200, "protocols", 0x04, NULL);
  
  // g_object_set(data.source, "location", "rtsp://admin:yangquan321@192.168.2.3:554/Streaming/Channels/101", \
  //                           "latency", 300, "protocols", 0x04, NULL);


  /* Connect to the pad-added signal */
  /* 在这里把回调函数的src data变量指定参数*/
  g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler),
      &data);
  
  /* Start playing */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return NULL;
  }

  /*GstPad* pad = gst_element_get_static_pad (data.convert, "sink");
  gst_pad_set_event_function(pad,gst_event_callback);*/
  
  /* Listen to the bus */
  bus = gst_element_get_bus (data.pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n",
              GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n",
              debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          terminate = TRUE;
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          terminate = TRUE;
          break;
        case GST_MESSAGE_STATE_CHANGED:
          /* We are only interested in state-changed messages from the pipeline */
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed (msg, &old_state, &new_state,
                &pending_state);
            g_message ("Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state),
                gst_element_state_get_name (new_state));
            char state_name[100];
            g_snprintf (state_name, 100, "%s", gst_element_state_get_name (new_state));
            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(data.pipeline), GST_DEBUG_GRAPH_SHOW_ALL, state_name);
          }
          break;
        default:
          /* We should not reach here */
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);

  return 0;
}

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

  GThread *egl_thread = g_thread_try_new ("test.egl.textrue", egl_thread_test_func, egl_context, NULL);

  // while (!g_atomic_int_compare_and_exchange (&wait_sync, 1, 0));
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
  glBindTexture(GL_TEXTURE_2D, texture[0]); 

  glUseProgram(shaderProgram);
  glBindVertexArray(VAO); 

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDrawArrays (GL_TRIANGLES, 0, 3);

  return TRUE;
}

static void
build_ui (GtkApplication *app) {

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
