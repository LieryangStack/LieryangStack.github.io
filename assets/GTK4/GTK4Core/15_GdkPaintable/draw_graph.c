#include <gtk/gtk.h>

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
                      double         rotation) {
          
  g_print ("snapshot = %p\n", snapshot);

  /* 将一个矩形区域（从 (0, 0) 到 (width, height)）填充为 background 颜色，并附加到 snapshot */
  // gtk_snapshot_append_color (snapshot,
  //                            &(GdkRGBA) { 0xEF/(float)0xFF, 0x96/(float)0xFF, 0xC5/(float)0xFF, 1 },
  //                            &GRAPHENE_RECT_INIT (0, 0, width, height));

  /* 使用渐变色填充背景色 */
  // GskColorStop stops[2];
  // stops[0].color = (GdkRGBA) { 0xCC/(float)0xFF, 0xFB/(float)0xFF, 0xFF/(float)0xFF, 1 };
  // stops[0].offset = 0.1;
  
  // stops[1].color = (GdkRGBA) { 0xEF/(float)0xFF, 0x96/(float)0xFF, 0xC5/(float)0xFF, 1 };
  // stops[1].offset = 0.5;

  // gtk_snapshot_append_linear_gradient (snapshot,
  //                                     &GRAPHENE_RECT_INIT (0, 0, width, height),
  //                                     &GRAPHENE_POINT_INIT (0, 0),
  //                                     &GRAPHENE_POINT_INIT (width, height),
  //                                     stops,
  //                                     2);


  /* 将之前的绘图操作保存在堆栈中，避免新的绘图操作影响之前的绘图内容 */
  gtk_snapshot_save (snapshot);

  /* 移动原点（起始点）到控件的中心 */
  // gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (width / 2.0, height / 2.0));
  gtk_snapshot_scale (snapshot, width, -height);
  /* 移动原点到 @point */
  gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT(0.0,  -1.0));
  gtk_snapshot_rotate (snapshot, rotation);





  /* 画圆心 */
  // builder = gsk_path_builder_new ();
  // gsk_path_builder_add_circle (builder, graphene_point_zero (), 0.5);
  // path = gsk_path_builder_free_to_path (builder);
  // gtk_snapshot_append_fill (snapshot, path, GSK_FILL_RULE_WINDING, foreground);
  // gsk_path_unref (path);

  GskStroke *stroke = gsk_stroke_new (0.005);
  gsk_stroke_set_line_join (stroke, GSK_LINE_JOIN_BEVEL);
  
  // gsk_stroke_set_dash (stroke, (float[1]) { RADIUS * G_PI }, 1); 
  GskPathBuilder *builder = gsk_path_builder_new ();
  // gsk_path_builder_add_circle (builder, graphene_point_zero(), RADIUS);
  // gsk_path_builder_move_to (builder, -1.0, 0.0);
  gsk_path_builder_line_to (builder, 0.1, 0.2);
  gsk_path_builder_line_to (builder, 0.2, 0.3);
  gsk_path_builder_line_to (builder, 0.3, 0.4);
  gsk_path_builder_line_to (builder, 0.4, 0.5);
  gsk_path_builder_line_to (builder, 0.5, 0.3);
  gsk_path_builder_line_to (builder, 0.8, 0.7);
  gsk_path_builder_line_to (builder, 1.0, 0.3);
  // gsk_path_builder_line_to (builder, 1.2, 0.0);

  GskPath *path = gsk_path_builder_free_to_path (builder);
  gtk_snapshot_append_stroke (snapshot, path, stroke, &(GdkRGBA) { 0.3, 0.4, 0.7, 1.0 });
  gtk_snapshot_append_fill (snapshot, path, GSK_FILL_RULE_WINDING, &(GdkRGBA) { 0.8, 0.984, 1.0, 1 });
  gsk_path_unref (path);
  gsk_stroke_free (stroke);


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

  gtk_widget_set_opacity (win, 0.75);

  gtk_window_present (GTK_WINDOW (win));

  GdkDisplay *diplay = gdk_display_get_default ();

  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  GdkGLContext *gl_context = gdk_gl_context_get_current ();

  g_timeout_add (1,
                  gtk_nuclear_animation_step,
                  nuclear);

  // g_print ("gl_context = %p\n", gl_context);
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application.draw.graph", G_APPLICATION_DEFAULT_FLAGS);
  
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
