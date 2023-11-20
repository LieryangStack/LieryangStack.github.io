---
layout: post
title: 一、GstStructure
categories: GStreamer核心对象
tags: [GStreamer]
---

## GstStructure

GstStructure 是一组键/值对的集合。键用 GQuarks 表示（GQuarks其实就是字符串，但是字符串和数字一一映射起来，为了字符串使用起来便利），而值可以是任何 GType 类型。

除了键/值对，GstStructure 还有一个名称。名称以字母开头，可以由字母、数字以及"/-_.:"中的任何字符组成。

GstStructure 被用于各种 GStreamer 子系统中，以一种灵活且可扩展的方式存储信息。

GstStructure 没有引用计数，因为它通常是更高级对象的一部分，例如 GstCaps、GstMessage、GstEvent、GstQuery。它提供了一种通过父级的引用计数来实施可变性的方法，可以使用 gst_structure_set_parent_refcount 方法。

可以使用 gst_structure_new_empty 或 gst_structure_new 创建 GstStructure。这两者都需要一个名称以及一组可选的键/值对和值的类型。

可以使用 gst_structure_set_value 或 gst_structure_set 来更改字段值。

可以使用 gst_structure_get_value 或更方便的 gst_structure_get_*() 函数来检索字段值。

可以使用 gst_structure_remove_field 或 gst_structure_remove_fields 来删除字段。

结构中的字符串必须是 ASCII 或 UTF-8 编码的。不允许使用其他编码。但字符串可以为 NULL。

## 函数总结

```c
/* 1.创建GstStructure结构 */
(1)只传入GstStructure名字，创建空成员结构体
GstStructure * gst_structure_new_empty (const gchar * name);
(2)可以传入名字和成员，创建结构体
GstStructure * gst_structure_new  (const gchar * name,
                                   const gchar * firstfield,
                                                          ...);
e.g
/*因为使用GVuale，需要指明成员类型，*/
test_structure = gst_structure_new("Family", "boy", G_TYPE_STRING, "Li",
                                             "girl", G_TYPE_STRING, "Zheng",NULL);

/* 2.获得GstStructure的名字和名字id（GQuark） */
const gchar *  gst_structure_get_name  (const GstStructure  * structure);
GQuark         gst_structure_get_name_id  (const GstStructure  * structure);

/* 3.设置GstStructure名字*/
void   gst_structure_set_name  (GstStructure        * structure,
                                const gchar         * name);
/* 4.添加成员 */
(1)添加单个成员
void  gst_structure_set_value  (GstStructure        * structure,
                                const gchar         * fieldname,
                                const GValue        * value);
(2)添加多个成员
void gst_structure_set  (GstStructure        * structure,
                         const gchar         * fieldname,
                                                          ...);
e.g
gst_structure_set(test_structure, "er", G_TYPE_STRING, "Apple",
                                  "yang", G_TYPE_STRING, "1996", NULL);

/* 5.获取单个成员信息 */
const GValue * gst_structure_get_value (const GstStructure  * structure,
                                        const gchar         * fieldname);

/* 6.遍历结构体成员 */
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
## 参考

[参考1：官方GstStructure](https://gstreamer.freedesktop.org/documentation/gstreamer/gststructure.html?gi-language=c#gst_structure_set)

[参考2：前辈总结Gstreamer-GstStructure](https://blog.csdn.net/knowledgebao/article/details/84937559)

写到最后，特别感谢[@knowledgebao](https://blog.csdn.net/knowledgebao)大佬关于Gstreamer和GObject学习总结，方便后辈学习。