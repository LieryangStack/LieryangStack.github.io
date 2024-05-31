---
layout: post
title: 四、GTK4——GtkListView
categories: GTK4
tags: [GTK4]
---

- GtkListView 显示一个大的动态项目列表。

- GtkListView 使用其工厂为每个可见项目生成一个行小部件，并在垂直或水平的线性显示中显示它们。

- GtkListView 属性提供了一种简单的方法来显示行之间的分隔符。

- GtkListView 允许用户根据模型的选择特性选择项目。对于允许多个选定项目的模型，可以使用 GtkListView 启用框选功能。

## 1 GListModel

1. `GListModel` 是一种接口（必须是GObject对象，才能去实现该接口）。

2. `GListModel` 表示可变 `GObject` 列表的接口。具有 `items-changed` 变化信号，列表中的项发生变化，会触发该信号。

3. `GListModel` 中的所有项都必须要来自同一种类型。

### 1.1 GtkStringList

1. `GtkStringList` 列表实现了 `GListModel` 模型接口。

2. `GtkStringList` 中的每一项都是 `GtkStringObject`，这是一个封装字符串的 `GObject` 对象。

    ```c
    struct _GtkStringObject
    {
      GObject parent_instance;
      char *string;
    };
    ```
