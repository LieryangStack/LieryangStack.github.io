---
layout: post
title: 三、GStreamer录像
categories: GStreamer项目
tags: [GStreamer项目]
---

gst-launch-1.0 v4l2src device="/dev/video0" ! videoconvert !  nvvideoconvert ! tee name=t \
t. ! nvh264enc ! h264parse ! qtmux ! filesink location=video.mp4 \
t. ! nveglglessink

gst-launch-1.0 -e v4l2src ! videoconvert ! nvvideoconvert ! queue ! timeoverlay ! nvh264enc ! h264parse ! splitmuxsink location=video%02d.mkv max-size-time=10000000000 muxer-factory=matroskamux muxer-properties="properties,streamable=true"


选择 YUY2 格式不会卡顿，原因是什么？？？

gst-launch-1.0 v4l2src device="/dev/video0" ! 'video/x-raw, width=640, height=480, format=YUY2'  ! videoconvert !  nvvideoconvert ! queue ! nveglglessink


