---
layout: post
title: 四、FFmpeg——AVFormatContext
categories: FFmpeg
tags: [FFmpeg]
---

在使用FFmpeg进行开发的时候，AVFormatContext是一个贯穿始终的数据结构，很多函数都要用它到作为参数。

- 封装格式信息: 该结构体包含了多媒体文件或流的封装格式信息, 如FLV、MP4等格式信息

- 管理音视频流: 该结构体管理多媒体文件中包含的多个流，如:视频流、音频流、字幕流，每个流都由AVStream结构体表示


使用 AVFormatContext 结构体 前需要导入头文件 :

```c
#include <libavformat/avformat.h> 
```

