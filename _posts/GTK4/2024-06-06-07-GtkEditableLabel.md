---
layout: post
title: 七、GTK4——GtkEditableLabel
categories: GTK4
tags: [GTK4]
---

## 1 GtkEditable

- `GtkEditable` 是一个用于文本编辑widget的接口。

- 典型的可编辑部件示例包括 `GtkEntry` 和 `GtkSpinButton`。它包含用于通用操作可编辑部件的函数、大量用于键绑定的操作信号以及应用程序可以连接以修改部件行为的几个信号。

作为后者用法的一个示例，通过将以下处理程序连接到 GtkEditable::insert-text 信号，应用程序可以将所有输入转换为大写。

