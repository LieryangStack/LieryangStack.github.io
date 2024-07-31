---
layout: post
title: 十、GTK4核心对象——GtkExpression
categories: GTK4核心对象
tags: [GTK4核心对象]
---

## 1 GtkExpression

1. `GtkExpression` 是一种基础类型，它不是 `GObject` 的子类，是直接在类型系统注册的一种类型，可以说类似 `GObject`。

2. `GtkConstantExpression`、`GtkPropertyExpression`等都是继承于 `GtkExpression` 定义的类型。

3. 通过 `gtk_expression_evaluate` 函数可以到创建表达式时候设定的值，如果创建表达式的时候有另外表达式，获取到的就是另外表达式的值，否则就是@this实例里面的值。