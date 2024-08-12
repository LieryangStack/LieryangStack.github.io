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

typedef struct _LineParams LineParams;
struct _LineParams {
  guint x1;
  guint y1;
  guint x2;
  guint y2;
};

LineParams line_params[16];
guint line_count= 0;
gboolean draw_line = TRUE;

static void
gtk_nuclear_icon_snapshot (GdkPaintable *paintable,
                           GdkSnapshot  *snapshot,
                           double        width,
                           double        height) {
                      
  GtkNuclearIcon *nuclear = GTK_NUCLEAR_ICON (paintable);

  /* 将之前的绘图操作保存在堆栈中，避免新的绘图操作影响之前的绘图内容 */
  gtk_snapshot_save (snapshot);

  /* 调整快照长宽范围到(1920, 1080) */
  gtk_snapshot_scale (snapshot, width/1920, height/1080);
  /* 移动原点到 @point */
  gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT(0.0,  0.0));


  GdkTexture *texture = gdk_texture_new_from_filename ("/home/lieryang/Desktop/截屏/摄像头0/2024-08-12CST20_13_04.jpg", NULL);
  gtk_snapshot_append_texture (snapshot, texture, &GRAPHENE_RECT_INIT(0, 0, 1920, 1080));

  GskStroke *stroke = gsk_stroke_new (5);
  gsk_stroke_set_line_join (stroke, GSK_LINE_JOIN_ROUND);
  
  GskPathBuilder *builder = gsk_path_builder_new ();

  for (guint i = 0; i < line_count; i++) {
    gsk_path_builder_move_to (builder, line_params[i].x1, line_params[i].y1);
    gsk_path_builder_line_to (builder, line_params[i].x2, line_params[i].y2);
  }

  GskPath *path = gsk_path_builder_free_to_path (builder);
  gtk_snapshot_append_stroke (snapshot, path, stroke, &(GdkRGBA) { 1.0, 0.0, 0.0, 1.0 });
  gsk_path_unref (path);
  gsk_stroke_free (stroke);

  /* 用于将之前保存的状态从堆栈中恢复 */
  gtk_snapshot_restore (snapshot);
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


/**
 * @brief: 判断两个点之间的距离是否小于 50
 */
static gboolean 
isDistanceLessThan50(gdouble x, gdouble y, gdouble x1, gdouble y1) {
  /* 计算两个点之间的平方距离 */
  float dx = x1 - x;
  float dy = y1 - y;
  float distanceSquared = dx * dx + dy * dy;

  /* 判断平方距离是否小于 50 的平方 */
  return distanceSquared < 2500.0f;
}


void
picture_area_pressed_cb (GtkGestureClick* self,
                         gint n_press,
                         gdouble x,
                         gdouble y,
                         gpointer user_data){
  
  GtkWidget *picture = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (self));

  x = (x / gtk_widget_get_width (picture) * 1920);
  y = (y / gtk_widget_get_height (picture) * 1080);
  
  // g_print ("pressed: (%0.2f, %0.2f)\n", x, y);

  if ((draw_line == FALSE && line_count > 2 ) || line_count == 16) {
    line_count = 0;
    draw_line = TRUE;
  }

  if (line_count > 2 && isDistanceLessThan50 (x, y, line_params[0].x1, line_params[0].y1)) {
    line_params[line_count- 1].x2 = (gint)line_params[0].x1;
    line_params[line_count- 1].y2 = (gint)line_params[0].y1;
    draw_line = FALSE;
  } else {
    line_params[line_count].x1 = x;
    line_params[line_count].y1 = y;
    line_params[line_count].x2 = x;
    line_params[line_count].y2 = y;
    line_count++;
  }

  /* 重新绘制 */
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (user_data));
}

void
picture_area_motion_cb (GtkEventControllerMotion* self,
                        gdouble x,
                        gdouble y,
                        gpointer user_data ) {
  
  GtkWidget *picture = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (self));

  x = (x / gtk_widget_get_width (picture) * 1920);
  y = (y / gtk_widget_get_height (picture) * 1080);

  // g_print ("motion: (%0.2f, %0.2f)\n", x, y);

  if (line_count > 0 && draw_line == TRUE) {
    line_params[line_count- 1].x2 = x;
    line_params[line_count- 1].y2 = y;
  }
   /* 重新绘制 */
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (user_data));
}


static void 
app_activate (GApplication *app, gpointer *user_data) {

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 1280, 720);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("标签");


  GdkPaintable *nuclear = gtk_nuclear_icon_new (0.0);
  // GtkWidget *image = gtk_image_new_from_paintable (nuclear); picture可以不受长宽比拉伸
  GtkWidget *picture = gtk_picture_new_for_paintable (nuclear);

  GtkEventController *motion = gtk_event_controller_motion_new ();
  GtkGesture *gesture = gtk_gesture_click_new ();
  gtk_widget_add_controller (picture, motion);
  gtk_widget_add_controller (picture, GTK_EVENT_CONTROLLER(gesture));

  g_signal_connect(gesture, "pressed", G_CALLBACK(picture_area_pressed_cb), nuclear);
  g_signal_connect(motion, "motion", G_CALLBACK(picture_area_motion_cb), nuclear);

  gtk_widget_set_hexpand (picture, TRUE);
  gtk_widget_set_vexpand (picture, TRUE);

  // gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), picture);
  gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  // gtk_widget_set_opacity (win, 0.75);

  gtk_window_present (GTK_WINDOW (win));

  GdkDisplay *diplay = gdk_display_get_default ();

  GdkSurface *surface = gtk_native_get_surface (gtk_widget_get_native (win));

  GdkGLContext *gl_context = gdk_gl_context_get_current ();

}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application.draw.graph", G_APPLICATION_DEFAULT_FLAGS);
  
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
