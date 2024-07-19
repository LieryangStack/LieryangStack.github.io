#define STB_IMAGE_IMPLEMENTATION

#include <gtk/gtk.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "stb_image.h"

#define GTK_TYPE_NUCLEAR_ICON (gtk_nuclear_icon_get_type ())
G_DECLARE_FINAL_TYPE (GtkNuclearIcon, gtk_nuclear_icon, GTK, NUCLEAR_ICON, GObject)

struct _GtkNuclearIcon
{
  GObject parent_instance;

  double rotation; /* 旋转值 */
};

struct _GtkNuclearIconClass
{
  GObjectClass parent_class;
};

static gint texture_id[2];

static gpointer 
thread_create_texture (gpointer data) {
  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress ("eglCreateImageKHR");
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC) eglGetProcAddress ("eglDestroyImageKHR");
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress ("glEGLImageTargetTexture2DOES");

  GdkGLContext *gdk_gl_context = gdk_display_create_gl_context(gdk_display_get_default(), NULL);

  gdk_gl_context_make_current (gdk_gl_context);

  EGLDisplay egl_display = eglGetCurrentDisplay ();
  EGLContext egl_context = eglGetCurrentContext ();
  EGLSurface egl_surface = eglGetCurrentSurface (EGL_DRAW);
  EGLint egl_config_id;
  eglQueryContext(egl_display, egl_context, EGL_CONFIG_ID, &egl_config_id);

  const EGLint imageAttributes[] = {
    EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
    EGL_NONE
  };

  /**
   * EGL_NATIVE_PIXMAP_KHR  像素图创建EGLImageKHR
   * EGL_LINUX_DMA_BUF_EXT  DMA缓冲区创建EGLImageKHR
   * EGL_GL_TEXTURE_2D_KHR  使用另一个纹理创建EGLImageKHR
   * 
  */
  EGLImageKHR image = eglCreateImageKHR (egl_display, egl_context, 
                                          EGL_GL_TEXTURE_2D_KHR,  
                                          (EGLClientBuffer)(uintptr_t)texture_id[0], 
                                          imageAttributes);
  if (image == EGL_NO_IMAGE_KHR) {
    g_print ("EGLImageKHR Error id = 0x%X \n", eglGetError());
  }

  glBindTexture(GL_TEXTURE_2D, texture_id[1]); 

  /**
   * GL_TEXTURE_2D
   * GL_TEXTURE_EXTERNAL_OES
  */
  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);

  eglDestroyImageKHR (egl_display, image);
  glBindTexture(GL_TEXTURE_2D, 0); 

  glFinish ();
}


/**
 * @brief: 图标的实际绘制函数
 */
void
gtk_nuclear_snapshot (GtkSnapshot   *snapshot,
                      const GdkRGBA *foreground,
                      const GdkRGBA *background,
                      double         width,
                      double         height,
                      double         rotation) {

  /* 将一个矩形区域（从 (0, 0) 到 (width, height)）填充为 background 颜色，并附加到 snapshot */
  gtk_snapshot_append_color (snapshot,
                             &(GdkRGBA) { 0, 0, 0, 1 },
                             &GRAPHENE_RECT_INIT (0, 0, width, height));

  /* 将之前的绘图操作保存在堆栈中，避免新的绘图操作影响之前的绘图内容 */
  gtk_snapshot_save (snapshot);

  /* 移动原点（起始点）到控件的中心 */
  // gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (width / 2.0, height / 2.0));
  gtk_snapshot_scale (snapshot, width, height);
  /* 移动原点到 @point */
  // gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT(0.0,  -1.0));
  // gtk_snapshot_rotate (snapshot, rotation);


  static gboolean flag = FALSE;

  GdkGLContext *context = gdk_gl_context_get_current ();

  if (flag == FALSE) {

    glGenTextures(2, texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0); 

    glBindTexture(GL_TEXTURE_2D, texture_id[1]); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0); 

    /* 绑定纹理 */
    glBindTexture(GL_TEXTURE_2D, texture_id[0]); 
    
    // 在加载图像之前设置翻转Y轴
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

    /* 解除纹理绑定 */
    glBindTexture(GL_TEXTURE_2D, 0); 

    GThread *thread = g_thread_try_new ("create-texture", thread_create_texture, NULL, NULL);
    g_thread_join (thread);

    flag = TRUE;
  }

  GdkGLTextureBuilder *builder = gdk_gl_texture_builder_new ();
  gdk_gl_texture_builder_set_context (builder, context);
  gdk_gl_texture_builder_set_id (builder, texture_id[1]);
  gdk_gl_texture_builder_set_width (builder, 1);
  gdk_gl_texture_builder_set_height (builder, 1);


  GdkTexture *texture = gdk_gl_texture_builder_build (builder,NULL, NULL);

  gtk_snapshot_append_texture (snapshot, texture, &GRAPHENE_RECT_INIT(0, 0, 1, 1));

  g_object_unref (builder);


  /* 用于将之前保存的状态从堆栈中恢复 */
  gtk_snapshot_restore (snapshot);

}


static void
gtk_nuclear_icon_snapshot (GdkPaintable *paintable,
                           GdkSnapshot  *snapshot,
                           double        width,
                           double        height)
{
  GtkNuclearIcon *nuclear = GTK_NUCLEAR_ICON (paintable);

  /* 实际去绘制图标的函数 */
  gtk_nuclear_snapshot (snapshot,
                        &(GdkRGBA) { 0, 0, 0, 1 }, /* black */
                        &(GdkRGBA) { 0.9, 0.75, 0.15, 1.0 }, /* yellow */
                        width, height,
                        nuclear->rotation);
}

static GdkPaintableFlags
gtk_nuclear_icon_get_flags (GdkPaintable *paintable)
{
  /*标志是非常有用的，让GTK知道这个图像是永远不会改变的。这允许许多优化，因此应该始终*设置。*/
  return GDK_PAINTABLE_STATIC_SIZE;
}

static void
gtk_nuclear_icon_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_nuclear_icon_snapshot;
  iface->get_flags = gtk_nuclear_icon_get_flags;
}

G_DEFINE_TYPE_WITH_CODE (GtkNuclearIcon, gtk_nuclear_icon, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                                gtk_nuclear_icon_paintable_init))

static void
gtk_nuclear_icon_class_init (GtkNuclearIconClass *klass)
{
}

static void
gtk_nuclear_icon_init (GtkNuclearIcon *nuclear)
{
}


GdkPaintable *
gtk_nuclear_icon_new (double rotation)
{
  GtkNuclearIcon *nuclear;

  nuclear = g_object_new (GTK_TYPE_NUCLEAR_ICON, NULL);
  nuclear->rotation = rotation;

  return GDK_PAINTABLE (nuclear);
}

static gboolean
gtk_nuclear_animation_step (gpointer data)
{
  GtkNuclearIcon *nuclear = data;

  nuclear->rotation = (nuclear->rotation + 0.1f) ;

  /* 重新绘制 */
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (nuclear));

  /* 下次循环继续迭代该事件源 */
  return G_SOURCE_CONTINUE;
}


static void 
app_activate (GApplication *app, gpointer *user_data) {

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 500, 400);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("标签");

  GdkPaintable *nuclear = gtk_nuclear_icon_new (0.0);
  // GtkWidget *image = gtk_image_new_from_paintable (nuclear); picture可以不受长宽比拉伸
  GtkWidget *picture = gtk_picture_new_for_paintable (nuclear);

  gtk_widget_set_hexpand (picture, TRUE);
  gtk_widget_set_vexpand (picture, TRUE);

  // gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), picture);
  // gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  // gtk_widget_set_opacity (win, 0.75);

  gtk_window_present (GTK_WINDOW (win));

  GdkDisplay *diplay = gdk_display_get_default ();

  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  GdkGLContext *gl_context = gdk_gl_context_get_current ();

  // g_timeout_add (1,
  //                 gtk_nuclear_animation_step,
  //                 nuclear);

  // g_print ("gl_context = %p\n", gl_context);
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application.Paintable", G_APPLICATION_DEFAULT_FLAGS);
  
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
