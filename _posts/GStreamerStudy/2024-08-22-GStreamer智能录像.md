---
layout: post
title: 四、GStreamer智能录像
categories: GStreamer项目
tags: [GStreamer项目]
---

# 1 GstInterpipe

interpipesink 和 interpipesrc 内部其实就是通过传输 GstBuffer，所以会造成事件无法相互传输。

# 2 

https://discourse.gstreamer.org/t/dynamically-adding-new-queue-to-a-tee-element/470

https://blog.csdn.net/qq_41563600/article/details/121343927

https://discourse.gstreamer.org/t/dynamically-record-video/1372

https://community.nxp.com/t5/i-MX-Processors/Dynamically-Record-Video/m-p/1850247?profile.language=ja