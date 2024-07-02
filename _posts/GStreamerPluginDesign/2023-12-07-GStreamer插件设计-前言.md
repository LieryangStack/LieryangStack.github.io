---
layout: post
title: GStreamer设计-前言
categories: GStreamer设计
tags: [GStreamer]
---

这部分内容来自：
- [GStreamer design documents](https://gstreamer.freedesktop.org/documentation/additional/design/index.html?gi-language=c)：里面讲述了各种GStreamer设计文档。这些文档是在开发或重构GStreamer设计部分时产生的技术文档，用来解决问题的设计解决方案。
- [Core Library](https://gstreamer.freedesktop.org/documentation/gstreamer/gi-index.html?gi-language=c)：里面讲述了GStreamer核心对象相关服务和功能，包括初始化、插件管理和类型，以及定义元素和bin的对象层次结构，以及一些更专门的元素。
- [GStreamer Writer's Guide](https://gstreamer.freedesktop.org/documentation/plugin-development/introduction/index.html?gi-language=c)：本章节旨在帮助您了解GStreamer框架，以便您可以开发新的插件来扩展现有功能。本指南通过跟踪一个用c编写的示例插件(一个音频过滤器插件)的开发来解决大多数问题。然而，本指南的后面部分还介绍了编写其他类型插件所涉及的一些问题，本指南的末尾描述了一些用于GStreamer的Python绑定。

我通过对以上章节内容的学习，把相关知识点进行详细总结。

NOTE：

- 我的`GStreamer设计`类别相关文章，写作顺序并未按照适合学习的逻辑顺序编写，如以后有时间，再按适合学习的逻辑顺序进行整理。
