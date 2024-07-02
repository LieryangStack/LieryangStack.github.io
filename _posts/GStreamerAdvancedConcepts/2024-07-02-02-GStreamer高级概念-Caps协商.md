---
layout: post
title: 二、GStreamer高级概念——Caps协商
categories: GStreamer高级概念
tags: [GStreamer高级概念]
---

Caps协商是在元素之间寻找它们可以处理的媒体格式（GstCaps）的过程。在大多数情况下，GStreamer 中的这个过程可以找到完整管道的最佳解决方案。在本节中，我们将解释这个过程是如何工作的。

## 1 Cpas negotiation basics

在GStreamer中，媒体格式的协商始终遵循以下简单规则：

- 下游元素建议一种格式在其sinkpad上，并将这一建议放置在对下游sinkpad执行的CAPS查询的结果中。也可参见“Implementing a CAPS query function”。

- 上游元素决定一种格式，这个上游元素并通过CAPS事件将所选的媒体格式发送到其source pad下游元素。下游元素重新配置自身以处理CAPS事件中的媒体类型。

- 下游元素可以通过向上游发送 `RECONFIGURE事件` 来通知上游，表明它希望建议一种新格式。RECONFIGURE事件简单地指示上游元素重新启动协商阶段。因为发送 `RECONFIGURE事件` 的元素现在建议另一种格式，所以管道中的格式可能会发生变化。

除了 `CAPS` 和 `RECONFIGURE事件` 以及 `CAPS查询` 之外，还有 `ACCEPT_CAPS查询`，用于快速检查元素是否可以接受某种caps。

所有协商都遵循这些简单规则。让我们看一些典型的用例以及协商的进行方式。

## 2 Caps negotiation use cases

接下来，我们将看一些push-mode调度的用例。pull-mode调度协商阶段在“Pull-mode Caps negotiation”中讨论，实际上与我们将看到的类似。

由于sink pads仅建议格式，<span style="background-color: pink;">而source pad需要做出决定</span>，因此最复杂的工作在source pad中完成。我们可以为source pad识别三种caps协商的用例：

- 固定协商Fixed negotiation。一个元素只能输出一种格式。请参见“Fixed negotiation”。

- 转换协商Transform negotiation。元素的输入和输出格式之间存在（固定的）转换，通常基于某个元素属性。元素的sink caps取决于上游src caps，元素的src caps取决于下游sink caps。请参见“Transform negotiation”。

- 动态协商Dynamic negotiation。一个元素可以输出多种格式。请参见“Dynamic negotiation”。

### 2.1 Fixed negotiation

在这种情况下，source pad只能生成一种固定格式。通常，这种格式已经编码在媒体内部。没有下游元素可以请求不同的格式，source pad重新协商的唯一方式是当元素决定自行更改caps时。

通常情况下，可以实现固定caps（在其source pad上）的元素是所有不可重新协商的元素。例如：

- 类型查找器，因为找到的类型是实际数据流的一部分，因此不能重新协商。类型查找器将检查字节流，确定类型，发送带有caps的CAPS事件，然后推送该类型的缓冲区。

- 几乎所有的demuxers，因为包含的基本数据流在文件头中已经定义，因此无法重新协商。

- 一些解码器，其中格式嵌入在数据流中，不是peercaps的一部分，而且解码器本身也不可重新配置。

- 一些生成固定格式的源。

对于具有固定caps的source pad，使用gst_pad_use_fixed_caps()。只要pad没有进行协商，默认的CAPS查询将返回垫模板中呈现的caps。一旦pad被协商，CAPS查询将返回协商的caps（而不是其他任何内容）。以下是固定caps source pad的相关代码片段。

```c
[..]
  pad = gst_pad_new_from_static_template (..);
  gst_pad_use_fixed_caps (pad);
[..]
```
可以通过调用gst_pad_set_caps()将固定的caps设置到pad上。
```c
[..]
    caps = gst_caps_new_simple ("audio/x-raw",
        "format", G_TYPE_STRING, GST_AUDIO_NE(F32),
        "rate", G_TYPE_INT, <samplerate>,
        "channels", G_TYPE_INT, <num-channels>, NULL);
    if (!gst_pad_set_caps (pad, caps)) {
      GST_ELEMENT_ERROR (element, CORE, NEGOTIATION, (NULL),
          ("Some debug information here"));
      return GST_FLOW_ERROR;
    }
[..]
```

这些类型的元素也没有输入格式和输出格式之间的关系，输入caps简单地不包含生成输出caps所需的信息。

所有其他需要为格式进行配置的元素应该实现完整的caps协商，这将在接下来的几节中进行解释。


### 2.2 Transform negotiation

在这种协商技术中，元素的输入 caps 和输出 caps 之间存在一个固定的转换。这种转换可以由元素属性参数化，但不能由流的内容参数化（用于这种用例，请参阅"Fixed negotiation"）。

元素能够接受的 caps 取决于（固定转换fixed transformation的）下游 caps。元素能够生成的 caps 取决于（固定转换fixed transformation的）上游 caps。

这种类型的元素通常可以在接收 CAPS 事件的情况下，从它的src pad 上的_event()函数中设置 caps，这意味着 caps 转换函数将一个固定的 caps 转换为另一个固定的 caps。元素的示例包括：

- Videobox。它根据对象属性添加可配置的边框，围绕视频帧。

- 身份元素Identity elements。所有不改变数据格式，只改变内容的元素。视频和音频效果是一个示例。其他示例包括检查流的元素。

- 一些解码器和编码器，其中输出格式由输入格式定义，如 mulawdec 和 mulawenc。这些解码器通常没有定义流内容的标头。它们通常更像是转换元素。

下面是一个典型转换元素的协商步骤示例。在 sink pad 的 CAPS 事件处理程序中，我们计算source pad 的 caps 并设置它们。

```c
  [...]

static gboolean
gst_my_filter_setcaps (GstMyFilter *filter,
               GstCaps *caps)
{
  GstStructure *structure;
  int rate, channels;
  gboolean ret;
  GstCaps *outcaps;

  structure = gst_caps_get_structure (caps, 0);
  ret = gst_structure_get_int (structure, "rate", &rate);
  ret = ret && gst_structure_get_int (structure, "channels", &channels);
  if (!ret)
    return FALSE;

  outcaps = gst_caps_new_simple ("audio/x-raw",
      "format", G_TYPE_STRING, GST_AUDIO_NE(S16),
      "rate", G_TYPE_INT, rate,
      "channels", G_TYPE_INT, channels, NULL);
  ret = gst_pad_set_caps (filter->srcpad, outcaps);
  gst_caps_unref (outcaps);

  return ret;
}

static gboolean
gst_my_filter_sink_event (GstPad    *pad,
                  GstObject *parent,
                  GstEvent  *event)
{
  gboolean ret;
  GstMyFilter *filter = GST_MY_FILTER (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      ret = gst_my_filter_setcaps (filter, caps);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

  [...]
```

### 2.3 Dynamic negotiation


最后一种协商方法是最复杂和强大的动态协商。

与“Transform negotiation”中的转换协商一样，动态协商将在下游/上游 caps 上执行转换。与转换协商不同，这种转换将固定的 caps 转换为非固定的 caps。这意味着 sink pad 的输入 caps 可以转换为非固定（多种）格式。src pad 必须从所有可能性中选择一个格式。通常情况下，它希望选择需要最少努力生成的格式，但不一定如此。格式的选择还应取决于下游可以接受的 caps（请参阅"Implementing a CAPS query function"中的QUERY_CAPS函数）。

典型的流程如下：

- 在元素的 sink pad 上接收 caps。

- 如果元素更喜欢以直通模式运行，请检查下游是否接受通过ACCEPT_CAPS查询获取的caps。如果是，我们可以完成协商，然后可以在直通模式下运行。

- 估算 pad 的可能 caps。

- 查询下游对等 pad 以获取可能 caps 的列表。

- 从下游列表中选择第一个可以转换为的 caps，并将其设置为输出 caps。您可能需要将 caps 固定为一些合理的默认值以构建固定 caps。

这种类型的元素示例包括：

- 转换元素，如videoconvert、audioconvert、audioresample、videoscale等...

- 源元素，如audiotestsrc、videotestsrc、v4l2src、pulsesrc等...

让我们看一个可以在采样率之间进行转换的元素的示例，因此输入和输出采样率不必相同：

```c

static gboolean
gst_my_filter_setcaps (GstMyFilter *filter,
               GstCaps *caps)
{
  if (gst_pad_set_caps (filter->srcpad, caps)) {
    filter->passthrough = TRUE;
  } else {
    GstCaps *othercaps, *newcaps;
    GstStructure *s = gst_caps_get_structure (caps, 0), *others;

    /* no passthrough, setup internal conversion */
    gst_structure_get_int (s, "channels", &filter->channels);
    othercaps = gst_pad_get_allowed_caps (filter->srcpad);
    others = gst_caps_get_structure (othercaps, 0);
    gst_structure_set (others,
      "channels", G_TYPE_INT, filter->channels, NULL);

    /* now, the samplerate value can optionally have multiple values, so
     * we "fixate" it, which means that one fixed value is chosen */
    newcaps = gst_caps_copy_nth (othercaps, 0);
    gst_caps_unref (othercaps);
    gst_pad_fixate_caps (filter->srcpad, newcaps);
    if (!gst_pad_set_caps (filter->srcpad, newcaps))
      return FALSE;

    /* we are now set up, configure internally */
    filter->passthrough = FALSE;
    gst_structure_get_int (s, "rate", &filter->from_samplerate);
    others = gst_caps_get_structure (newcaps, 0);
    gst_structure_get_int (others, "rate", &filter->to_samplerate);
  }

  return TRUE;
}

static gboolean
gst_my_filter_sink_event (GstPad    *pad,
                  GstObject *parent,
                  GstEvent  *event)
{
  gboolean ret;
  GstMyFilter *filter = GST_MY_FILTER (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      ret = gst_my_filter_setcaps (filter, caps);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

static GstFlowReturn
gst_my_filter_chain (GstPad    *pad,
             GstObject *parent,
             GstBuffer *buf)
{
  GstMyFilter *filter = GST_MY_FILTER (parent);
  GstBuffer *out;

  /* push on if in passthrough mode */
  if (filter->passthrough)
    return gst_pad_push (filter->srcpad, buf);

  /* convert, push */
  out = gst_my_filter_convert (filter, buf);
  gst_buffer_unref (buf);

  return gst_pad_push (filter->srcpad, out);
}
```

## 3 Upstream caps (re)negotiation

上游协商的主要用途是重新协商（部分）已经协商的管道以适应新的格式。一些实际的示例包括选择不同的视频尺寸，因为视频窗口的大小发生了变化，而视频输出本身无法重新缩放，或者因为音频通道配置发生了变化。

向上游发出caps重新协商的请求是通过向上游发送GST_EVENT_RECONFIGURE事件来完成的。其想法是，它将指示上游元素通过进行新的允许caps的查询然后选择新的caps来重新配置其caps。发送RECONFIGURE事件的元素将通过从其GST_QUERY_CAPS查询函数返回新的首选caps来影响新caps的选择。RECONFIGURE事件将在其经过的所有pads上设置GST_PAD_FLAG_NEED_RECONFIGURE。

需要注意的是，不同的元素实际上在这里有不同的责任：

- 希望向上游提议新格式的元素需要首先使用ACCEPT_CAPS查询检查新caps是否可以在上游接受。然后，它们将发送RECONFIGURE事件，并准备好用新的首选格式回答CAPS查询。应该注意的是，当没有上游元素可以（或者愿意）重新协商时，元素需要处理当前配置的格式。

- 根据转换协商运作的元素（按照“转换协商”）将RECONFIGURE事件传递给上游。由于这些元素只是基于上游caps执行固定的转换，因此它们需要将事件传递给上游，以便上游可以选择新格式。

- 进行固定协商的元素（固定协商）会丢弃RECONFIGURE事件。这些元素无法重新配置，它们的输出caps不依赖于上游caps，因此可以丢弃该事件。

- 可以在source pad上重新配置的元素（实现动态协商的src pad在“Dynamic negotiation”中有详细说明）应该使用gst_pad_check_reconfigure()检查其NEED_RECONFIGURE标志，当该函数返回TRUE时应开始重新协商。

## 4 Implementing a CAPS query function

当对等元素想要了解此垫支持的格式以及首选顺序时，会调用具有GST_QUERY_CAPS查询类型的_query()函数。返回值应该是该元素支持的所有格式，考虑到下游或上游的对等元素的限制，并按首选顺序排序，首选顺序最高的排在前面。

```c

static gboolean
gst_my_filter_query (GstPad *pad, GstObject * parent, GstQuery * query)
{
  gboolean ret;
  GstMyFilter *filter = GST_MY_FILTER (parent);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_CAPS
    {
      GstPad *otherpad;
      GstCaps *temp, *caps, *filt, *tcaps;
      gint i;

      otherpad = (pad == filter->srcpad) ? filter->sinkpad :
                                           filter->srcpad;
      caps = gst_pad_get_allowed_caps (otherpad);

      gst_query_parse_caps (query, &filt);

      /* We support *any* samplerate, indifferent from the samplerate
       * supported by the linked elements on both sides. */
      for (i = 0; i < gst_caps_get_size (caps); i++) {
        GstStructure *structure = gst_caps_get_structure (caps, i);

        gst_structure_remove_field (structure, "rate");
      }

      /* make sure we only return results that intersect our
       * padtemplate */
      tcaps = gst_pad_get_pad_template_caps (pad);
      if (tcaps) {
        temp = gst_caps_intersect (caps, tcaps);
        gst_caps_unref (caps);
        gst_caps_unref (tcaps);
        caps = temp;
      }
      /* filter against the query filter when needed */
      if (filt) {
        temp = gst_caps_intersect (caps, filt);
        gst_caps_unref (caps);
        caps = temp;
      }
      gst_query_set_caps_result (query, caps);
      gst_caps_unref (caps);
      ret = TRUE;
      break;
    }
    default:
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}

```

## 5 Pull-mode Caps negotiation

“WRITEME” 表示文档中的某部分尚未编写或尚未完全理解。根据本章中提供的所有知识，您应该能够编写一个执行正确的caps协商的元素。如果有疑问，可以查看我们的Git仓库中相同类型的其他元素，以了解它们如何实现您想要完成的操作。

## 参考

[翻译自：Caps negotiation](https://gstreamer.freedesktop.org/documentation/plugin-development/advanced/negotiation.html?gi-language=c)