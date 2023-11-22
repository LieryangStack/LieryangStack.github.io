---
layout: post
title: 一、GstStructure
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstStructure 基本概念

GstStructure 是一组键/值对的集合。键用 GQuarks 表示（GQuarks其实就是字符串，但是字符串和数字一一映射起来，为了字符串使用起来便利），而值可以是任何 GType 类型。

除了键/值对，GstStructure 还有一个名称。名称以字母开头，可以由字母、数字以及"/-_.:"中的任何字符组成。

GstStructure 被用于各种 GStreamer 子系统中，以一种灵活且可扩展的方式存储信息。

GstStructure 没有引用计数，因为它通常是更高级对象的一部分，例如 GstCaps、GstMessage、GstEvent、GstQuery。它提供了一种通过父级的引用计数来实施可变性的方法，可以使用 gst_structure_set_parent_refcount 方法。

可以使用 gst_structure_new_empty 或 gst_structure_new 创建 GstStructure。这两者都需要一个名称以及一组可选的键/值对和值的类型。

可以使用 gst_structure_set_value 或 gst_structure_set 来更改字段值。

可以使用 gst_structure_get_value 或更方便的 gst_structure_get_*() 函数来检索字段值。

可以使用 gst_structure_remove_field 或 gst_structure_remove_fields 来删除字段。

结构中的字符串必须是 ASCII 或 UTF-8 编码的。不允许使用其他编码。但字符串可以为 NULL。

## 2 序列化格式

序列化的字符串主要是在 `gst_structure_from_string` 函数中使用。后面也会在创建结构体章节中讲到该函数的具体使用方法。

GstStructure的序列化格式将GstStructure的名称、键/GType/值序列化为逗号分隔的列表，其中结构名称作为第一个字段，没有值，其后跟着的是以key=value形式分开的键/值对。例如：

```c
"我的结构体name, key=value"
```

如果没有明确指定，值的类型将会被推断，使用(GTypeName)value语法。例如，以下结构将有一个名为'is-string'的field，其值为字符串'true'：

```c
"a-struct, field-is-string=(string)true, field-is-boolean=true"
```

**注意**：

- 如果不指定(string)，field-is-string的类型将被推断为布尔型。

- 我们指定(string)为类型，即使gchararray是实际的string名称，为了方便，一些知名类型已被别名化或缩写。

- 当结构通过 `gst_structure_to_string` 序列化时，所有值都会被明确地类型化。

为了避免指定类型，你可以给“类型系统”一些提示。例如，要指定一个值为double，你应该添加一个小数点（即1是一个int，而1.0是一个double）。

**一些类型有特殊的分隔符**：

- GstValueArray在花括号 `{` 和 `}`内。例如a-structure, array={1, 2, 3}

- 范围在方括号 `[` 和 `]`内。例如a-structure, range=[1, 6, 2] 其中1是最小值，6是最大值，2是步长。要指定GST_TYPE_INT64_RANGE，你需要明确指定它，如：a-structure, a-int64-range=(gint64) [1, 5]

- GstValueList在"小于和大于"符号 `<` 和 `>` 内。例如`a-structure, list=<1, 2, 3>

结构体由空字符 `\0` 或分号 `;` 分隔，后者允许在同一个字符串中存储多个结构（见GstCaps）。

`引号`被用作"默认"分隔符，可以围绕任何不使用其他分隔符的类型（例如a-struct, i=(int)"1"）。它们用于允许在字符串内添加空格或特殊字符（如分隔符、分号等），并且可以使用反斜杠 `\`来转义其中的字符，例如：

```c
"a-struct, special="\"{[(;)]}\" can be used inside quotes""
```

它们还允许嵌套结构，例如：

```c
"a-struct, nested=(GstStructure)"nested-struct, nested=true""
```

自1.20版本起，嵌套结构和caps可以使用方括号 `[` 和 `]` 指定，例如：

```c
"a-struct, nested=[nested-struct, nested=true]"
```

**注意**:由于向后兼容性的原因，`gst_structure_to_string` 不会使用该语法，为此目的添加了 `gst_structure_serialize`。

## 3 GBoxed 类型

通过查看源文件和头文件，可以发现 `GstStructure` 是GBoxed类型，所以源文件定义了创建 `GstStructure` 内存函数 `gst_structure_new_empty` 或 `gst_structure_new`。但是本质上申请的是 `GstStructureImpl` 的结构体内存。

释放结构体 `GstStructure` 内存函数 `gst_structure_free`。

```c
/* filename: gststructure.h */
struct _GstStructure {
  GType type;

  /*< private >*/
  GQuark name;
};

/* filename: gststructure.c */
/**
 * 源文件下面这两个结构体对外都是私有的，外部不能访问，只能通过函数操作
 * 每个键值对存储结构体 
 */
struct _GstStructureField
{
  GQuark name;
  GValue value;
};

/* GstStructureImpl才是最重要的结构体，因为该结构体的第一个成员是GstStructure，内存申请也是申请该结构体占用内存 */
typedef struct
{
  GstStructure s;

  /* owned by parent structure, NULL if no parent */
  gint *parent_refcount;

  guint fields_len;             /* Number of valid items in fields */
  guint fields_alloc;           /* Allocated items in fields */

  /* Fields are allocated if GST_STRUCTURE_IS_USING_DYNAMIC_ARRAY(),
   *  else it's a pointer to the arr field. */
  GstStructureField *fields;

  GstStructureField arr[1];
} GstStructureImpl;

G_DEFINE_BOXED_TYPE (GstStructure, gst_structure,
    gst_structure_copy_conditional, gst_structure_free);
```

## 4 函数总结

对于常用的一些 `GstStructure` 函数进行总结。

### 4.1 GstStructure 创建函数

#### 4.1.1 gst_structure_new_empty

以下两个函数都是创建零个键值对`GstStructure`，只是一个使用字符串，一个使用GQuark。

**注意**：`GstStructure`必须有`name`。

```c
/*只传入GstStructure名字，创建空成员结构体，没有键值对*/
GstStructure *gst_structure_new_empty (const gchar * name);

GstStructure *gst_structure_new_id_empty (GQuark quark);
```

#### 4.1.2 gst_structure_new

```c
GstStructure *gst_structure_new  (const gchar * name,
                                  const gchar * firstfield,
                                  ...);


GstStructure *gst_structure_new_valist  (const gchar * name,
                                         const gchar * firstfield,
                                         va_list       varargs);

GstStructure *gst_structure_new_id  (GQuark name_quark,
                                     GQuark field_quark,
                                     ...);
```

示例

```c
test_structure = gst_structure_new("Family", 
                                   "boy", G_TYPE_STRING, "Li",
                                   "age", G_TYPE_INT, 24, NULL);

guint64 latency = 100;

structure = gst_structure_new_id (GST_QUARK (EVENT_LATENCY),
                                  GST_QUARK (LATENCY), G_TYPE_UINT64, latency, NULL);
```

gst_structure_new_valist 测试程序在 [/assets/GStreamerStudy/CoreObject/01_GstStrucure/gst_structure_new_valist.c](/assets/GStreamerStudy/CoreObject/01_GstStrucure/gst_structure_new_valist.c)

#### 4.1.3 gst_structure_from_string

`gst_structure_from_string` 是从序列化字符串创建Gst结构体。两个函数的区别就是 `end` 参数，如果 `end`参数不为空，最后返回的`end`为序列化字符串被处理完的地址。只要序列化字符串被正常处理完，end = string + strlen (string)。

```c
GstStructure *
gst_structure_from_string  (const gchar * string,
                            gchar      ** end);

GstStructure *
gst_structure_new_from_string (const gchar * string)
{
  return gst_structure_from_string (string, NULL);
}
```

### 4.2 获得GstStructure的名字

```c
/* 获得GstStructure的名字和名字id（GQuark） */
const gchar *  gst_structure_get_name  (const GstStructure  * structure);
GQuark         gst_structure_get_name_id  (const GstStructure  * structure);
```

### 4.3 设置GstStructure名字

```c
/* 3.设置GstStructure名字*/
void   gst_structure_set_name  (GstStructure        * structure,
                                const gchar         * name);
```

### 4.4 GstStructure添加键值对

```c
//(1)添加单个成员
void  gst_structure_set_value  (GstStructure        * structure,
                                const gchar         * fieldname,
                                const GValue        * value);
//(2)添加多个成员
void gst_structure_set  (GstStructure        * structure,
                         const gchar         * fieldname,
                                                          ...);
//e.g
gst_structure_set(test_structure, "er", G_TYPE_STRING, "Apple",
                                  "yang", G_TYPE_STRING, "1996", NULL);
```

### 4.5 GstStructure获取单个成员信息
```c
const GValue * gst_structure_get_value (const GstStructure  * structure,
                                        const gchar         * fieldname);
```
### 4.6 GstStructure遍历结构体成员
```c
gboolean gst_structure_foreach (const GstStructure  * structure,
                                GstStructureForeachFunc   func,
                                gpointer              user_data);
e.g
gboolean
print_value(GQuark   field_id,
            const GValue * value,
            gpointer user_data){
  /*一定返回TRUE，返回FALSE只会遍历第一个*/
  return TRUE;
}
```

### 4.7 序列化GstStructure

```c
gchar *
gst_structure_to_string (const GstStructure * structure)
```

### 4.8 释放GstStructure

因为不是标准的 `GObject` 对象，所以不会自动调用free函数，所以必须手动调用 `gst_structure_free`。

```c
void
gst_structure_free (GstStructure * structure);
```

## 参考

[参考1：官方GstStructure](https://gstreamer.freedesktop.org/documentation/gstreamer/gststructure.html?gi-language=c#gst_structure_set)
