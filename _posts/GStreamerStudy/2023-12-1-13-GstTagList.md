---
layout: post
title: 十三、GstTagList
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstTagList基本概念

- 用于描述媒体metadata元数据的标签tag和值value的列表。

- 结构中的字符串必须是ASCII或UTF-8编码。不允许使用其他编码。字符串不得为空或为%NULL。

## 2 GstTagList类型结构

### 2.1 GstTagList类型注册宏定义

```c
/* filename:  gsttaglist.h   */
GST_API GType _gst_tag_list_type;
#define GST_TYPE_TAG_LIST     (_gst_tag_list_type)

/* filename:  gsttaglist.c   */
GType _gst_tag_list_type = 0;
GST_DEFINE_MINI_OBJECT_TYPE (GstTagList, gst_tag_list);
```

### 2.2 GstTagList类型相关枚举

#### 2.2.1 GstTagMergeMode

```c
/* filename:  gsttaglist.h   */
  
/**
 * GstTagMergeMode:
 * @GST_TAG_MERGE_UNDEFINED: undefined merge mode
 * @GST_TAG_MERGE_REPLACE_ALL: replace all tags (clear list and append)
 * @GST_TAG_MERGE_REPLACE: replace tags
 * @GST_TAG_MERGE_APPEND: append tags
 * @GST_TAG_MERGE_PREPEND: prepend tags
 * @GST_TAG_MERGE_KEEP: keep existing tags
 * @GST_TAG_MERGE_KEEP_ALL: keep all existing tags
 * @GST_TAG_MERGE_COUNT: the number of merge modes
 *
 * 不同的标签合并模式基本上有替换、覆盖和追加，但可以从两个方向看待。
 * 考虑到两个标签列表：(A)元素中已经存在的标签和(B)提供给元素的标签
 * 例如，通过gst_tag_setter_merge_tags() / gst_tag_setter_add_tags()或%GST_EVENT_TAG），这些标签如何合并？
 * 在下表中，显示了标签存在于列表(A)或不存在于列表(!A)以及它们的组合情况。
 *
 * | merge mode  | A + B | A + !B | !A + B | !A + !B |
 * | ----------- | ----- | ------ | ------ | ------- |
 * | REPLACE_ALL | B     | ø      | B      | ø       |
 * | REPLACE     | B     | A      | B      | ø       |
 * | APPEND      | A, B  | A      | B      | ø       |
 * | PREPEND     | B, A  | A      | B      | ø       |
 * | KEEP        | A     | A      | B      | ø       |
 * | KEEP_ALL    | A     | A      | ø      | ø       |
 */

typedef enum {
  GST_TAG_MERGE_UNDEFINED,
  GST_TAG_MERGE_REPLACE_ALL,
  GST_TAG_MERGE_REPLACE,
  GST_TAG_MERGE_APPEND,
  GST_TAG_MERGE_PREPEND,
  GST_TAG_MERGE_KEEP,
  GST_TAG_MERGE_KEEP_ALL,
  /* add more */
  GST_TAG_MERGE_COUNT
} GstTagMergeMode;
```

#### 2.2.2 GstTagFlag

```c
/* filename:  gsttaglist.h   */

/**
 * GstTagFlag:
 * @GST_TAG_FLAG_UNDEFINED: undefined flag
 * @GST_TAG_FLAG_META: tag is meta data
 * @GST_TAG_FLAG_ENCODED: tag is encoded
 * @GST_TAG_FLAG_DECODED: tag is decoded
 * @GST_TAG_FLAG_COUNT: number of tag flags
 *
 * Extra tag flags used when registering tags.
 */
/* FIXME: these are not really flags .. */
typedef enum {
  GST_TAG_FLAG_UNDEFINED,
  GST_TAG_FLAG_META,
  GST_TAG_FLAG_ENCODED,
  GST_TAG_FLAG_DECODED,
  GST_TAG_FLAG_COUNT
} GstTagFlag;
```

#### 2.2.3 GstTagScope

```c
/**
 * GstTagScope:
 * @GST_TAG_SCOPE_STREAM: tags被用于单一的流stream
 * @GST_TAG_SCOPE_GLOBAL: global tags被用于完整的medium
 *
 * GstTagScope specifies if a taglist applies to the complete
 * medium or only to one single stream.
 */
typedef enum {
  GST_TAG_SCOPE_STREAM,
  GST_TAG_SCOPE_GLOBAL
} GstTagScope;
```

### 2.3 GstTagList相关结构体

#### 2.3.1 GstTagList

```c
/* filename:  gsttaglist.h   */
typedef struct _GstTagList GstTagList;
struct _GstTagList {
  GstMiniObject mini_object;
};
```

#### 2.3.2 GstTagInfo

```c
/* filename:  gsttaglist.c   */
typedef struct
{
  GType type;                   /* type the data is in */

  const gchar *nick;            /* translated short description */
  const gchar *blurb;           /* translated long description  */

  GstTagMergeFunc merge_func;   /* functions to merge the values */
  GstTagFlag flag;              /* type of tag */
  GQuark name_quark;            /* quark for the name */
}
GstTagInfo;
```

#### 2.3.3 GstTagListImpl

```c
/* filename:  gsttaglist.c   */

typedef struct _GstTagListImpl
{
  GstTagList taglist;

  GstStructure *structure;
  GstTagScope scope;
} GstTagListImpl;
```

## 3 GstTagList对象相关函数

