---
layout: post
title: 三、GStreamer视频时间查询
categories: GStreamer项目
tags: [GStreamer项目]
---


# 1 查询duration

[示例代码：/assets/GStreamerStudy/04_time_duration/查询时间.c](/assets/GStreamerStudy/04_time_duration/查询时间.c)

我们使用的播放文件 [/assets/GStreamerStudy/04_time_duration/sample_720p.h264](/assets/GStreamerStudy/04_time_duration/sample_720p.h264)

该文件的大小为 `14759548` bytes

1. 对于h264流文件，并没有准确的 `duration` 时间（我们管道不能获取到准确的时间，得到的只是估计时间）。如果使用了qtdemux这些元素，通过这些元素可以获取准确duration.

2. 对整个管道进行duration查询的时候，会把查询事件发送给最后的sink元素，然后通过pad依次调用。

<font color='red'>
整个管道可以没有固定duration，最终的播放是通过
</font>

![](/assets/GStreamerStudy/04_time_duration/查询duration分析.png)

# 2 查询position


