---
layout: post
title: 二十一、GstPadTemplate
categories: GStreamer核心对象
tags: [GStreamer]
---

## 1 GstPadTemplate基本概念

- Padtemplates描述了一个pad或element factory可以处理的可能媒体类型media types。这使得在加载元素插件Element plugin之前可以检查处理的类型，以及识别尚未创建（请求Request或有时的Sometimes）元素上的pads成为可能。

- Pad和PadTemplates附加了GstCaps来描述它们能够处理的媒体类型。使用gst_pad_template_get_caps或GST_PAD_TEMPLATE_CAPS可以获取padtemplate的caps。创建后不可能修改padtemplate的caps。

- PadTemplates有一个GstPadPresence属性，用于标识pad的生命周期，可以使用GST_PAD_TEMPLATE_PRESENCE进行检索。此外，可以使用GST_PAD_TEMPLATE_DIRECTION从GstPadTemplate中检索pad的方向。

- GST_PAD_TEMPLATE_NAME_TEMPLATE（）对于GST_PAD_REQUEST pads非常重要，因为它必须作为gst_element_request_pad_simple调用中的名称，从而从该模板实例化一个pad。

- 可以使用gst_pad_template_new或gst_static_pad_template_get（）创建padtemplate，后者可以使用便捷的GST_STATIC_PAD_TEMPLATE宏填充一个GstStaticPadTemplate，从而创建一个GstPadTemplate。

- 可以使用padtemplate来创建一个pad（参见gst_pad_new_from_template或gst_pad_new_from_static_template（））或添加到一个元素类（参见gst_element_class_add_static_pad_template（））。

- 以下代码示例显示了从padtemplate创建pad的代码：

  ```c
  GstStaticPadTemplate my_template =
    GST_STATIC_PAD_TEMPLATE (
      "sink",          // the name of the pad
      GST_PAD_SINK,    // the direction of the pad
      GST_PAD_ALWAYS,  // when this pad will be present
      GST_STATIC_CAPS (        // the capabilities of the padtemplate
        "audio/x-raw, "
          "channels = (int) [ 1, 6 ]"
      )
    );
    void
    my_method (void)
    {
      GstPad *pad;
      pad = gst_pad_new_from_static_template (&my_template, "sink");
      ...
    }
  ```

- 下面的例子展示了如何将padtemplate添加到元素类中，这通常在类的class_init中完成:

  ```c
  static void
  my_element_class_init (GstMyElementClass *klass)
  {
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

    gst_element_class_add_static_pad_template (gstelement_class, &my_template);
  }
  ```
### 1.1 GstPadTemplate 继承关系

```c
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstPadTemplate
```

## 2 GstPadTemplate类型结构

### 2.1 GstPadTemplate类型注册宏定义

```c
/* filename: gstpadtemplate.h */
#define GST_TYPE_PAD_TEMPLATE		(gst_pad_template_get_type ())
/* filename: gstpadtemplate.c */
G_DEFINE_TYPE (GstPadTemplate, gst_pad_template, GST_TYPE_OBJECT);
```

### 2.2 GstPadTemplate类型相关枚举

#### 2.2.1 GstPadPresence

```c
typedef enum {
  GST_PAD_ALWAYS, /* Pad总是可获得的 */
  GST_PAD_SOMETIMES, /* Pad将会变得可获得的，依赖于媒体流 */
  GST_PAD_REQUEST /* Pad只有在请求的时候可获得 */
} GstPadPresence;
```

### 2.3 GstPadTemplate相关结构体

#### 2.3.1 GstPadTemplateClass

```c
/* filename: gstpadtemplate.h */
struct _GstPadTemplateClass {
  GstObjectClass   parent_class;

  /* signal callbacks */
  void (*pad_created)	(GstPadTemplate *templ, GstPad *pad);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};
```

#### 2.3.2 GstPadTemplate

```c
/* filename: gstpadtemplate.h */
struct _GstPadTemplate {
  GstObject	   object;

  gchar           *name_template;
  GstPadDirection  direction;
  GstPadPresence   presence;
  GstCaps	  *caps;

  /*< private >*/
  union {
    gpointer _gst_reserved[GST_PADDING];
    struct {
      GType gtype;
      GstCaps *documentation_caps;
    } abi;
  } ABI;
};
```

### 2.3.3 GstStaticPadTemplate

```c
/* filename: gstpadtemplate.h */
struct _GstStaticPadTemplate {
  const gchar     *name_template;
  GstPadDirection  direction;
  GstPadPresence   presence;
  GstStaticCaps    static_caps;
};

/* filename: gstpadtemplate.c */
G_DEFINE_POINTER_TYPE (GstStaticPadTemplate, gst_static_pad_template);
```

## 3 GstStaticPadTemplate

```c
/* filename: gststaticpadtemplate.h */
struct _GstStaticPadTemplate {
  const gchar     *name_template;
  GstPadDirection  direction;
  GstPadPresence   presence;
  GstStaticCaps    static_caps;
};
```


## 4 GstPadTemplate对象相关函数