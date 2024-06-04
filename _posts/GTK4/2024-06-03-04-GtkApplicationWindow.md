---
layout: post
title: 四、GTK4——GtkApplicationWindow
categories: GTK4
tags: [GTK4]
---

## 1 GtkApplication

```c
/* filename: gtkapplication.c */
G_DEFINE_TYPE_WITH_PRIVATE (GtkApplication, gtk_application, G_TYPE_APPLICATION)
```

### 1.1 GApplicationClass类startup信号处理函数重写code

对`GApplication`对象中的默认启动信号，添加了默认处理函数。该函数主要执行了 `gtk_init()`。

```c
static void
gtk_application_startup (GApplication *g_application)
{
  ...

  gtk_init ();
  
  ...
}

static void
gtk_application_class_init (GtkApplicationClass *class) {

  ...

  application_class->startup = gtk_application_startup;

  /* 下面会介绍这两个默认处理函数 */
  class->window_added = gtk_application_window_added;
  class->window_removed = gtk_application_window_removed;

  ...
}

```

### 1.2 该类新增信号 window-added、window-removed

#### 1.2.1 window-added

```c
/**
 * 窗口新增信号，默认处理函数
*/
static void
gtk_application_window_added (GtkApplication *application,
                              GtkWindow      *window) {

...

/* application->priv->use_count++ */
g_application_hold (G_APPLICATION (application)); /* 应用程序可以进行迭代循环了 */

...

}
```

通过把当前窗口添加到应用程序，使得应用程序可以进行迭代事件循环。

![alt text](/assets/GTK4/03_GtkApplicationWindow/image/image-1.png)

#### 1.2.2 window-removed

```c
static void
gtk_application_window_removed (GtkApplication *application,
                                GtkWindow      *window) {
...

g_application_release (G_APPLICATION (application));

...
}
```

## 2 GtkWindow

### 2.1 GtkWindow关闭请求信号close-request

![alt text](/assets/GTK4/03_GtkApplicationWindow/image/image-3.png)

GtkWindow类初始化函数中：

```c
static void
gtk_window_class_init (GtkWindowClass *klass)
{
  ...

  /* 这是默认的关闭请求处理函数 */
  klass->close_request = gtk_window_close_request;


  window_signals[CLOSE_REQUEST] =
    g_signal_new (I_("close-request"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtkWindowClass, close_request),
                  _gtk_boolean_handled_accumulator, NULL,
                  _gtk_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN,
                  0);
  g_signal_set_va_marshaller (window_signals[CLOSE_REQUEST],
                              GTK_TYPE_WINDOW,
                              _gtk_marshal_BOOLEAN__VOIDv);

}
```

如果我应用程序连接了 `close-request` 处理函数，处理函数如果返回FALSE，`gtk_window_close_request`将会执行；如果我的处理函数返回TRUE，`gtk_window_close_request`不会执行。

### 2.2 GtkWindow引用

所有继承`GtkWindow`对象，初始创建后，都是正常引用，因为`GtkWindow`实例初始化函数中，会把浮点引用转换成正常引用。

```c
/**
 * 初始化过程中，转换成正常引用
*/
static void
gtk_window_init (GtkWindow *window)
{
  ...

  g_object_ref_sink (window);
  ...
}

/**
 * 调用 gtk_window_destroy 函数，引用计数减一
*/
void
gtk_window_destroy (GtkWindow *window)
{
  guint i;

  g_return_if_fail (GTK_IS_WINDOW (window));

  /* If gtk_window_destroy() has been called before. Can happen
   * when destroying a dialog manually in a ::close handler for example. */
  if (!g_list_store_find (toplevel_list, window, &i))
    return;


  g_object_ref (window);

  gtk_tooltip_unset_surface (GTK_NATIVE (window));

  gtk_window_hide (GTK_WIDGET (window));

  gtk_accessible_update_state (GTK_ACCESSIBLE (window),
                               GTK_ACCESSIBLE_STATE_HIDDEN, TRUE,
                               -1);

  /* 这个函数里面会进行引用计数减一操作 */
  g_list_store_remove (toplevel_list, i);

  gtk_window_release_application (window);

  gtk_widget_unrealize (GTK_WIDGET (window));

  g_object_unref (window);
}

```
 
### 2.3 close和destroy函数区别

1. `gtk_window_close` 函数不一定会关闭，因为会根据关闭请求处理函数返回值，决定是否执行 `gtk_window_destroy`。
2. `gtk_window_destroy` 函数一定会关闭窗口。

```c
void
gtk_window_close (GtkWindow *window)
{
  GtkWindowPrivate *priv = gtk_window_get_instance_private (window);

  if (!_gtk_widget_get_realized (GTK_WIDGET (window)))
    return;

  if (priv->in_emit_close_request)
    return;

  g_object_ref (window);

  if (!gtk_window_emit_close_request (window))
    gtk_window_destroy (window);

  g_object_unref (window);
}
```


## 3 GtkApplicationWindow

1. 不难发现，相比 `GtkWindow`，明显是实现了Action接口。

```c
/* filename: gtkapplicationwindow.h */
struct _GtkApplicationWindow
{
  GtkWindow parent_instance;
};

/**
 * GtkApplicationWindowClass:
 * @parent_class: The parent class.
 */
struct _GtkApplicationWindowClass
{
  GtkWindowClass parent_class;

  /*< private >*/
  gpointer padding[8];
};

/* filename: gtkapplicationwindow.c */
G_DEFINE_TYPE_WITH_CODE (GtkApplicationWindow, gtk_application_window, GTK_TYPE_WINDOW,
                         G_ADD_PRIVATE (GtkApplicationWindow)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ACTION_GROUP, gtk_application_window_group_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ACTION_MAP, gtk_application_window_map_iface_init))
```

## 4 Gtk启动应用程序窗口分析

![alt text](/assets/GTK4/03_GtkApplicationWindow/image/image-2.png)

