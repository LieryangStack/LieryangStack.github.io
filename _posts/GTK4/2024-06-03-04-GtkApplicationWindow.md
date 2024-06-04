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

![alt text](image-1.png)

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

## 2 GtkApplicationWindow

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

## 3 Gtk启动应用程序窗口分析

![alt text](image-2.png)