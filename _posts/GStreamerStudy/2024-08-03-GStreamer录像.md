---
layout: post
title: 三、GStreamer录像
categories: GStreamer项目
tags: [GStreamer项目]
---

gst-launch-1.0 v4l2src -e device="/dev/video0" ! videoconvert !  nvvideoconvert ! tee name=t \
t. ! nvh264enc ! h264parse ! qtmux ! filesink location=video.mp4 \
t. ! nveglglessink

gst-launch-1.0 -e v4l2src ! videoconvert ! nvvideoconvert ! queue ! timeoverlay ! nvh264enc ! h264parse ! splitmuxsink location=video%02d.mkv max-size-time=10000000000 muxer-factory=matroskamux muxer-properties="properties,streamable=true"

gst-launch-1.0 -e v4l2src ! videoconvert ! nvvideoconvert ! queue ! timeoverlay ! nvh264enc ! h264parse ! qtmux ! filesink location=video.mp4

选择 YUY2 格式不会卡顿，原因是什么？？？

gst-launch-1.0 -e v4l2src device="/dev/video0" ! 'video/x-raw, width=640, height=480, format=YUY2'  ! tee name=t \
t. ! videoconvert ! nvvideoconvert ! queue ! nveglglessink \
t. ! videoconvert ! nvvideoconvert ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=received_h264.mkv

nvv4l2h264enc 和 nvh264enc 区别




### 播放rtsp码流视频

gst-launch-1.0 rtspsrc location=rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101 protocols=0x04 latency=50 ! rtph264depay ! h264parse ! tee ! avdec_h264 ! nvvideoconvert ! nveglglessink

### 播放rtsp码流视频 + 录像

#### 使用avdec_h264解码

gst-launch-1.0 -e rtspsrc location=rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101 protocols=0x04 latency=300 ! \
rtph264depay ! h264parse ! tee name=t \
t. ! queue ! avdec_h264 ! nvvideoconvert ! nveglglessink \
t. ! queue ! matroskamux ! filesink location=received_h264.mkv

#### 使用nvv4l2decoder解码

gst-launch-1.0 -e rtspsrc location=rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101 protocols=0x04 latency=300 ! \
rtph264depay ! tee name=t \
t. ! h264parse ! video/x-h264,stream-format=byte-stream,alignment=au ! nvv4l2decoder ! nvvideoconvert ! nveglglessink \
t. ! h264parse ! video/x-h264,stream-format=avc,alignment=au ! matroskamux ! filesink location=received_h264.mkv