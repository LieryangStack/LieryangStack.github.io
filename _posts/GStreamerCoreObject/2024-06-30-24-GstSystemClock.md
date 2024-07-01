---
layout: post
title: 二十四、GstSystemClock
categories: GStreamer核心对象
tags: [GStreamer]
---

- GstClock是抽象对象，GstSystemClock是抽象对象GstClock的实现。

- gst_system_clock_obtain 函数用来获取Gst系统内部定义的时钟，也就是 `static GstClock *_the_system_clock = NULL`。该变量是由Gst系统内部创建，不能释放，是一个静态对象。

- gst_clock_get_time 函数表示获取当前系统开机运行了多长时间（因为内部创建的时候，使用的FLAG是GST_CLOCK_TYPE_MONOTONIC）。gst_clock_get_time 会根据校准变量，修改 gst_clock_get_internal_time 获取到的时间。

- gst_clock_get_internal_time 函数表示获取当前系统开机运行了多长时间（因为内部创建的时候，使用的FLAG是GST_CLOCK_TYPE_MONOTONIC）。跟gst_clock_get_time的区别是，如果进行了 gst_clock_set_calibration 操作，就会有区别，否则没有区别。

- gst_clock_set_calibration 可以设置校准变量。

  ![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image-2.png)

- 如果设置了 `GST_OBJECT_FLAG_SET (clock, GST_CLOCK_FLAG_NEEDS_STARTUP_SYNC)`，就可以使用 `gst_clock_wait_for_sync` 阻塞等待时钟同步。

## 1 GstClock和GstSystemClock关系

![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image.png)

## 2 GstSystemClock获取时间的类型

![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image-3.png)

可以参考：[/assets/GStreamerCoreObject/24_GstSystemClock/01_设置时钟类型.c](/assets/GStreamerCoreObject/24_GstSystemClock/01_设置时钟类型.c)

![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image-4.png)

## 3 GstClock时钟校准

```c
/**
 * @brief: 直接获取时间，未根据校准公式修改时间
*/
GstClockTime
gst_clock_get_internal_time (GstClock * clock);


/**
 * @brief: 先直接获取未调整时间，然后再根据校准公式修改时间，最后返回修改后的时间
*/
GstClockTime
gst_clock_get_time (GstClock * clock);
```

<font color="red">其实只要不设定校准值，默认情况下 `gst_clock_get_internal_time` 和 `gst_clock_get_time` 是没有区别的。</font>

### 3.1 防止读时间期间，修改校准值

![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image-5.png)

![alt text](/assets/GStreamerCoreObject/24_GstSystemClock/image/image-6.png)

## 4 GstClock同步

```c
/**
 * @brief: 必须先设置了 GST_CLOCK_FLAG_NEEDS_STARTUP_SYNC， 才会根据 @timeout 阻塞等待同步。
 * @note: 如果没设置 GST_CLOCK_FLAG_NEEDS_STARTUP_SYNC，将立即返回
*/
gboolean
gst_clock_wait_for_sync (GstClock * clock, GstClockTime timeout);
```

