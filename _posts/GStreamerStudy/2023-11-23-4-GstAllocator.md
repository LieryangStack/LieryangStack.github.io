---
layout: post
title: 四、GstAllocator
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstAllocator基本概念

`GstAllocator` 是用来创建内存 `GstMemory`。

内存（GstMemory）通常是通过分配器使用 `gst_allocator_alloc` 方法调用创建的。当使用 NULL 作为分配器时，将使用默认的分配器。

可以通过 `gst_allocator_register` 注册新的分配器。分配器通过名称来识别，并且可以通过 `gst_allocator_find` 检索。可以使用 `gst_allocator_set_default` 来更改默认分配器。

可以使用 `gst_memory_new_wrapped` 创建新的内存，该方法包装在其他地方分配的内存。

**注意**：

1. `GstAllocator`是一个抽象对象，意味着它只能被继承，本身不能实例化。所以肯定有一个能够实例化的继承于`GstAllocator`对象。

2. `GstAllocator` 是用来创建 `GstMemory` 对象。

3. gstallocator.c源文件先实现了一个`GstAllocator`抽象对象，然后基于`GstAllocator`实现了一个默认的系统内存分配器`GstAllocatorSysmem`。

### 1.1 GstAllocator继承关系

```c
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstAllocator
```

## 2 GstAllocator类型结构

### 2.1 GstAllocator类型注册宏定义



```c
/* gstallocator.h */
#define GST_TYPE_ALLOCATOR                 (gst_allocator_get_type())
/* gstallocator.c */
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstAllocator, gst_allocator,
    GST_TYPE_OBJECT);
```
### 2.2 GstAllocator类型相关枚举定义

#### 2.2.1 GstAllocatorFlags

```c
/* gstallocator.h */
/**
 * GstAllocatorFlags:
 * @GST_ALLOCATOR_FLAG_CUSTOM_ALLOC: 分配器具有自定义的分配函数。
 * @GST_ALLOCATOR_FLAG_LAST: 可以用于自定义目的的第一个标志
 *
 * 分配器的标志。
 */
typedef enum {
  GST_ALLOCATOR_FLAG_CUSTOM_ALLOC  = (GST_OBJECT_FLAG_LAST << 0),

  GST_ALLOCATOR_FLAG_LAST          = (GST_OBJECT_FLAG_LAST << 16)
} GstAllocatorFlags;
```

### 2.3 GstAllocator类型相关结构体定义

#### 2.3.1 GstAllocatorClass类结构体

创建`GstMemory`和释放`GstMemory`函数的实现。

- `gst_allocator_alloc` 调用 `aclass->alloc` 函数

- `gst_allocator_free` 调用 `aclass->free` 函数

```c
/* gstallocator.h */
/**
 * GstAllocatorClass:
 * @object_class: 父类对象
 * @alloc: 用于获取内存的实现，被`gst_allocator_alloc`函数调用。
 * @free: 用于释放内存的实现,被`gst_allocator_free`函数调用。
 *
 * #GstAllocator 用于创建新内存。
 */
struct _GstAllocatorClass {
  GstObjectClass object_class;

  /*< public >*/
  GstMemory *  (*alloc)      (GstAllocator *allocator, gsize size,
                              GstAllocationParams *params);
  void         (*free)       (GstAllocator *allocator, GstMemory *memory);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.2 GstAllocatorClass实例结构体

`gst_allocator_init` 初始化函数中 `allocator->priv = gst_allocator_get_instance_private (allocator);`


```c
/* gstallocator.h */
/**
 * GstAllocator:
 * @name: 分配器的名称
 * @mem_map: GstMemoryMapFunction 的实现，这个函数的实现包含的用户存储数据指针的映射，被gst_memory_map函数调用。
 * @mem_unmap: GstMemoryUnmapFunction 的实现，这个函数的实现包含的用户存储数据指针的解映射，被gst_memory_unmap函数调用。
 * @mem_copy: GstMemoryCopyFunction 的实现，拷贝GstMemory数据到一个新的GstMemory，被gst_memory_copy函数调用。
 * @mem_share: GstMemoryShareFunction 的实现，被gst_memory_share函数调用。
 * @mem_is_span: GstMemoryIsSpanFunction 的实现，合并两个GstMemory，被gst_memory_is_span函数调用。
 * @mem_map_full: GstMemoryMapFullFunction 的实现，和mem_map实现的功能类似，优先使用mem_map_full。
 *      如果存在，将代替 @mem_map 使用。（自版本 1.6 起）
 * @mem_unmap_full: GstMemoryUnmapFullFunction 的实现,和mem_map实现的功能类似，优先使用mem_unmap_full。
 *      如果存在，将代替 @mem_unmap 使用。（自版本 1.6 起）
 *
 * #GstAllocator 用于创建新内存。
 */
struct _GstAllocator
{
  GstObject  object;

  const gchar               *mem_type;

  /*< public >*/
  GstMemoryMapFunction       mem_map;
  GstMemoryUnmapFunction     mem_unmap;

  GstMemoryCopyFunction      mem_copy;
  GstMemoryShareFunction     mem_share;
  GstMemoryIsSpanFunction    mem_is_span;

  GstMemoryMapFullFunction   mem_map_full;
  GstMemoryUnmapFullFunction mem_unmap_full;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING - 2];

  GstAllocatorPrivate *priv;
};
```

#### 2.3.3 GstAllocationParams

```c
/* gstallocator.h */
/**
 * GstAllocationParams:
 * @flags: 控制内存分配的标志
 * @align: 期望的内存对齐方式，必须是2的幂次减一
 * @padding: 期望的填充
 *
 * 用于控制内存分配的参数
 */
struct _GstAllocationParams {
  GstMemoryFlags flags;
  gsize          align;
  gsize          prefix;
  gsize          padding;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.4 GstAllocatorPrivate
```c
/* gstallocator.c */
struct _GstAllocatorPrivate
{
  gpointer dummy;
};
```
## 3 GstAllocator对象相关函数总结

### 3.1 GstAllocationParams结构体相关

```c
/* 申请内存空间，创建一个GstAllocationParams结构体 */
GstAllocationParams *
gst_allocation_params_new (void);

/* 拷贝GstAllocationParams结构体函数 */
GstAllocationParams *
gst_allocation_params_copy (const GstAllocationParams * params);

/* 释放GstAllocator */
void
gst_allocation_params_free (GstAllocationParams * params)
```

### 3.2 GstAllocator注册查找设定默认分配器

```c
void
gst_allocator_register(const gchar * name, GstAllocator * allocator);

GstAllocator *
gst_allocator_find (const gchar * name);

void
gst_allocator_set_default (GstAllocator * allocator);
```

### 3.3 GstAllocator创建和释放GstMemory对象

这两个函数的关键都是调用GstAllocator类的虚函数`alloc`和`free`

```c
GstMemory *
gst_allocator_alloc (GstAllocator * allocator, gsize size,
                     GstAllocationParams * params);

void
gst_allocator_free (GstAllocator * allocator, GstMemory * memory);
```

## 4 GstAllocatorSysmem对象的实现

### 4.1 GstMemorySystem

```c
typedef struct
{
  GstMemory mem;

  gsize slice_size;
  guint8 *data;

  gpointer user_data;
  GDestroyNotify notify;
} GstMemorySystem;
```

### 4.2 GstAllocatorSysmem实例和类结构体

```c
typedef struct
{
  GstAllocator parent;
} GstAllocatorSysmem;

typedef struct
{
  GstAllocatorClass parent_class;
} GstAllocatorSysmemClass;
```

### 4.3 GstAllocatorSysmem类型注册函数

```c
static GType gst_allocator_sysmem_get_type (void);
G_DEFINE_TYPE (GstAllocatorSysmem, gst_allocator_sysmem, GST_TYPE_ALLOCATOR);
```

### 4.4 GstAllocatorSysmem实现类和实例中的函数指针

