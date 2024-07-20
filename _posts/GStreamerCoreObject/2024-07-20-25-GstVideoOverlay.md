---
layout: post
title: 二十五、GstVideoOverlay
categories: GStreamer核心对象
tags: [GStreamer]
---

GstVideoOverlay 接口主要用于两个目的：

1. 获取视频接收器元素将渲染视频的窗口。这可以通过以下两种方式实现：要么通知视频接收器元素生成的窗口标识符，要么强制视频接收器元素使用特定的窗口标识符进行渲染。

2. 强制重新绘制视频接收器元素在窗口上显示的最新视频帧。实际上，如果 GstPipeline 处于 GST_STATE_PAUSED 状态，移动窗口会损坏其内容。应用程序开发人员将希望自己处理暴露事件并强制视频接收器元素刷新窗口的内容。


使用视频接收器创建的窗口可能是最简单的场景，但在某些情况下，如果应用程序开发人员需要捕获诸如鼠标移动和按钮点击之类的事件，这可能不够灵活。

在视频接收器元素上设置特定的窗口标识符是最灵活的解决方案，但它有一些问题。实际上，应用程序需要在正确的时间设置其窗口标识符，以避免视频接收器元素内部创建窗口。为了解决这个问题，会在总线上发布一个 GstMessage，通知应用程序应立即设置窗口标识符。以下是如何正确执行此操作的示例：


```c
static GstBusSyncReply
 create_window (GstBus * bus, GstMessage * message, GstPipeline * pipeline)
 {
  // ignore anything but 'prepare-window-handle' element messages
  if (!gst_is_video_overlay_prepare_window_handle_message (message))
    return GST_BUS_PASS;

  win = XCreateSimpleWindow (disp, root, 0, 0, 320, 240, 0, 0, 0);

  XSetWindowBackgroundPixmap (disp, win, None);

  XMapRaised (disp, win);

  XSync (disp, FALSE);

  gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (GST_MESSAGE_SRC (message)),
      win);

  gst_message_unref (message);

  return GST_BUS_DROP;
 }
 ...
 int
 main (int argc, char **argv)
 {
 ...
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_set_sync_handler (bus, (GstBusSyncHandler) create_window, pipeline,
        NULL);
 ...
 }
```