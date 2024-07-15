#define STB_IMAGE_IMPLEMENTATION

#include <gtk/gtk.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


#include "stb_image.h"
static GtkWidget *window = NULL;


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


/**
 * @brief: 图标的实际绘制函数
 */
void
gtk_nuclear_snapshot (GtkSnapshot   *snapshot,
                      const GdkRGBA *foreground,
                      const GdkRGBA *background,
                      double         width,
                      double         height,
                      double         rotation)
{
#define RADIUS 0.3
  // GskPathBuilder *builder;
  GskPath *path;
  GskStroke *stroke;
  double size;

  /* 将一个矩形区域（从 (0, 0) 到 (width, height)）填充为 background 颜色，并附加到 snapshot */
  // gtk_snapshot_append_color (snapshot,
  //                            background,
  //                            &GRAPHENE_RECT_INIT (0, 0, width, height));

  size = MIN (width, height);

  gtk_snapshot_save (snapshot);

  /* 绘制坐标变化到中心位置 */
  // gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (width / 2.0, height / 2.0));
  // gtk_snapshot_scale (snapshot, size, size);
  // gtk_snapshot_rotate (snapshot, rotation);

  gint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  // // 在加载图像之前设置翻转Y轴
  // stbi_set_flip_vertically_on_load(true); 已经在着色器中修改纹理坐标了
  int img_width, img_height, nrChannels;
  // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
  unsigned char *img_data = stbi_load("image/one.png", \
                                  &img_width, &img_height, &nrChannels, 0);
  if (img_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
      glGenerateMipmap(GL_TEXTURE_2D);
  } else {
      g_print ("Failed to load texture\n");
  }

  glBindTexture(GL_TEXTURE_2D, 0); 


  GdkGLContext *context = gdk_gl_context_get_current ();
  

  GdkGLTextureBuilder *builder = gdk_gl_texture_builder_new ();
  gdk_gl_texture_builder_set_context (builder, context);
  gdk_gl_texture_builder_set_id (builder, texture_id);
  gdk_gl_texture_builder_set_width (builder, img_width);
  gdk_gl_texture_builder_set_height (builder, img_height);

  GdkTexture *texture = gdk_gl_texture_builder_build (builder,NULL, NULL);

  gtk_snapshot_append_texture (snapshot, texture, &GRAPHENE_RECT_INIT(
                                     0, 0,
                                     img_width,
                                     img_height
                                   ));

  g_object_unref (builder);

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

  /* Add 1 to the progress and reset it when we've reached
   * the maximum value.
   * The animation will rotate by 360 degrees at MAX_PROGRESS
   * so it will be identical to the original unrotated one.
   */
  nuclear->rotation = (nuclear->rotation + 0.1f) ;

  /* Now we need to tell all listeners that we've changed out contents
   * so that they can redraw this paintable.
   */
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (nuclear));

  /* We want this timeout function to be called repeatedly,
   * so we return this value here.
   * If this was a single-shot timeout, we could also
   * return G_SOURCE_REMOVE here to get rid of it.
   */
  return G_SOURCE_CONTINUE;
}

GtkWidget *
do_paintable (GtkWidget *do_widget)
{
  GdkPaintable *nuclear;
  GtkWidget *image;

  if (!window)
    {
      window = gtk_window_new ();
      gtk_window_set_display (GTK_WINDOW (window),
                              gtk_widget_get_display (do_widget));
      gtk_window_set_title (GTK_WINDOW (window), "Nuclear Icon");
      gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);
      g_object_add_weak_pointer (G_OBJECT (window), (gpointer *)&window);

      nuclear = gtk_nuclear_icon_new (0.0);
      image = gtk_image_new_from_paintable (nuclear);
      gtk_window_set_child (GTK_WINDOW (window), image);
      g_object_unref (nuclear);
    }

    g_timeout_add (1,
                                      gtk_nuclear_animation_step,
                                      nuclear);

  if (!gtk_widget_get_visible (window))
    gtk_widget_set_visible (window, TRUE);
  else
    gtk_window_destroy (GTK_WINDOW (window));

  return window;
}


GtkWidget *win;

static void 
app_activate (GApplication *app, gpointer *user_data) {

  win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 500, 400);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("小标签");

  GtkWidget *popover = gtk_popover_new ();

  gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  gtk_window_present (GTK_WINDOW (win));

  GdkDisplay *diplay = gdk_display_get_default ();

  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  GdkGLContext *gl_context = gdk_gl_context_get_current ();

  // g_print ("gl_context = %p\n", gl_context);
  do_paintable (win);
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("gtk.application.paintable", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
