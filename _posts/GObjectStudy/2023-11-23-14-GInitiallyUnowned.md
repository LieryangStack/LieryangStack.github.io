---
layout: post
title: 十四、GInitiallyUnowned类型
categories: GObject学习笔记
tags: [GObject]
---

## 1 GInitiallyUnowned基本概念

一个用于具有初始浮动引用的对象的类型。该对象继承于`GObject`，除了实例初始化的时候把该对象初始位浮点引用外，其余都和`GObject`对象一样。

- 我们在GTK库或者其他库的时候，使用继承 `GInitiallyUnowned` 的GTK对象，处于浮点状态的时候，不能直接解引用。

- 浮点引用状态调用 `g_object_ref_sink` ，浮点引用状态变成正常引用状态，引用数不变。

- 正常引用状态调用 `g_object_ref_sink` ，引用数 `+1`。

## 2 GInitiallyUnowned结构体与定义

通过查看 `gobject` 源文件和头文件，不难发现 `GInitiallyUnowned` 和 `GObject` 类结构体和实例结构体完全相同，唯一的区别就是 `GInitiallyUnowned` 对象初始化的时候调用了强制浮点引用函数。

```c
/* filename: gobject.h */

typedef struct _GObject                  GInitiallyUnowned;
typedef struct _GObjectClass             GInitiallyUnownedClass;

/* filename: gobject.c */

G_DEFINE_TYPE (GInitiallyUnowned, g_initially_unowned, G_TYPE_OBJECT)

static void
g_initially_unowned_init (GInitiallyUnowned *object)
{
  g_object_force_floating (object);
}

static void
g_initially_unowned_class_init (GInitiallyUnownedClass *klass)
{
}

```

## 3 GInitiallyUnowned浮点引用

强制浮点引用函数操作的是实例结构体中的 `GData *qdata` 成员变量，该变量应该是被用来存储GData数据，也就是用户需要挂载自定义关联数据到该对象。<span style="color:red;">~~虽然进行其他操作的时候应该都会先把浮点引用变成正常引用，但是我觉得这样还是有安全风险。~~</span>

其实并不会出现这样的问题，是我肤浅了。

`object_floating_flag_handler` 实际处理浮点引用标记位的函数。

```c
/* filename: gobject.c */

static guint (*floating_flag_handler) (GObject*, gint) = object_floating_flag_handler;

void
g_object_force_floating (GObject *object)
{
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (g_atomic_int_get (&object->ref_count) >= 1);

  floating_flag_handler (object, +1);
}

/**
 * @brief: 标记浮点引用的关键函数
 *         其实就是操作 object->qdata 的内容，标记 object->qdata | 0x02 就是标记了浮点引用
*/
static guint
object_floating_flag_handler (GObject        *object,
                              gint            job)
{
  switch (job)
    {
      gpointer oldvalue;
    case +1:    /* force floating if possible */
      do
        oldvalue = g_atomic_pointer_get (&object->qdata);
      while (!g_atomic_pointer_compare_and_exchange ((void**) &object->qdata, oldvalue,
                                                     (gpointer) ((gsize) oldvalue | OBJECT_FLOATING_FLAG)));
      return (gsize) oldvalue & OBJECT_FLOATING_FLAG;
    case -1:    /* sink if possible */
      do
        oldvalue = g_atomic_pointer_get (&object->qdata);
      while (!g_atomic_pointer_compare_and_exchange ((void**) &object->qdata, oldvalue,
                                                     (gpointer) ((gsize) oldvalue & ~(gsize) OBJECT_FLOATING_FLAG)));
      return (gsize) oldvalue & OBJECT_FLOATING_FLAG;
    default:    /* check floating */
      return 0 != ((gsize) g_atomic_pointer_get (&object->qdata) & OBJECT_FLOATING_FLAG);
    }
}

```

### 3.1 如何让指针变量既能存储地址又能做Flag

通过测试程序 [/assets/GObjectStudy/202311/14_GInitiallyUnowned/GInitiallyUnowned.c](/assets/GObjectStudy/202311/14_GInitiallyUnowned/GInitiallyUnowned.c) 不难发现，我们创建浮点引用对象后，直接设定qdata，然后浮点引用变成正常，从对象取出qdata。完全没有任何问题。

我们设定qdata和获取qdata指针的时候，经过 `G_DATALIST_GET_POINTER` 和 `G_DATALIST_SET_POINTER` 宏。可以获取包含Flag的指针地址（此时地址不是真正的地址，不能直接使用）。

```c
/* filename: gdataset.c */

#define G_DATALIST_FLAGS_MASK_INTERNAL 0x7

/* datalist pointer accesses have to be carried out atomically */
#define G_DATALIST_GET_POINTER(datalist)						\
  ((GData*) ((gsize) g_atomic_pointer_get (datalist) & ~(gsize) G_DATALIST_FLAGS_MASK_INTERNAL))

#define G_DATALIST_SET_POINTER(datalist, pointer)       G_STMT_START {                  \
  gpointer _oldv, _newv;                                                                \
  do {                                                                                  \
    _oldv = g_atomic_pointer_get (datalist);                                            \
    _newv = (gpointer) (((gsize) _oldv & G_DATALIST_FLAGS_MASK_INTERNAL) | (gsize) pointer);     \
  } while (!g_atomic_pointer_compare_and_exchange ((void**) datalist, _oldv, _newv));   \
} G_STMT_END
```



