---
layout: post
title: 十、GstCapsFeatures
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstCapsFeatures基本概念

- GstCapsFeatures 可以选择性地设置在 GstCaps 上，以添加对特定 GstStructure 的额外特性要求。具有相同名称但带有非等值特性集的 Caps 结构不兼容。如果一个 pad 支持多组特性，它必须添加多个相同的结构，但具有不同的特性集到 caps 中。

- 空的 GstCapsFeatures 等同于仅包含 GST_CAPS_FEATURE_MEMORY_SYSTEM_MEMORY 的 GstCapsFeatures。任何由 gst_caps_features_new_any 创建的 ANY GstCapsFeatures 与其他任何 GstCapsFeatures 相等，并可用于指定支持任何 GstCapsFeatures，例如对于不处理缓冲区内存的元素。带有 ANY GstCapsFeatures 的 GstCaps 被视为非固定的，在协商过程中必须选择某些 GstCapsFeatures。

- 特性的示例可能包括对特定 GstMemory 类型的要求，或对缓冲区上必须有特定 GstMeta 的要求。特性以字符串形式给出，格式为 memory:GstMemoryTypeName 或 meta:GstMetaAPIName。

## 2 GstCapsFeatures类型结构

### 2.1 GstCapsFeatures类型注册宏定义

```c
/* filename: gstcapsfeatures.h */

GST_API GType _gst_caps_features_type;

#define GST_TYPE_CAPS_FEATURES (_gst_caps_features_type)

/* filename: gstcapsfeatures.c */
GType _gst_caps_features_type = 0;

G_DEFINE_BOXED_TYPE (GstCapsFeatures, gst_caps_features,
    gst_caps_features_copy, gst_caps_features_free);

```

### 2.2 GstCapsFeatures相关结构体

#### 2.2.1 GstCapsFeatures

```c
/* filename: gstcapsfeatures.c */
struct _GstCapsFeatures
{
  GType type;
  gint *parent_refcount;
  GArray *array; /* 存储GQuark */
  gboolean is_any; /* 表示ANY类型的GstCapsFeatures */
};
```

## 3 GstCapsFeatures对象相关函数