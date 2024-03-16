---
layout: post
title: 十五、Meson构建文件函数——subdir()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 subdir()

进入指定的子目录并执行其中的 meson.build 文件。一旦完成，它将返回并继续执行 subdir() 命令之后的内容。在子目录 meson.build 文件中定义的变量（子目录的 meson.build执行完毕之后），之后所有的meson.build中文件都可以使用。

请注意，这意味着源代码树中的每个 meson.build 文件只能且必须执行一次。

<font color='red'>1. 子目录的 meson.build 文件没有 project 函数（子项目的构建文件才有project） </font>

<font color='red'>2. 子目录的 meson.build 中定义变量，后续的主构建文件或者子目录构建文件都可以直接使用（也就相当于include的意思，导入了） </font>

## 2 subdir()定义

```python
# Enters the specified subdirectory and executes the `meson
void subdir(
  str dir_name,     # Directory relative to the current `meson

  # Keyword arguments:
  if_found : list[dep]  # Only enter the subdir if all dep.found() methods return `true`.
)
```

## 3 subdir()示例

示例程序目录：[/assets/BuildSystem/Meson/15_WriteMesonFile/](/assets/BuildSystem/Meson/15_WriteMesonFile/)

**主构建文件**：

```python
project('my_project', 'cpp', version: '1.0')

# 执行子构建
subdir('libsimple')

gtk_dep = dependency('gtk+-3.0')

# 子构建的 libsimple_dep 可以直接使用
executable('demo',
           'main.cpp',
           dependencies : [libsimple_dep, gtk_dep],
           install : true)
```

**子构建文件**：

```python
# project('libsimple', 'cpp', version: '1.0')

inc = include_directories('include')

# 生成动态库 libsimple.so
libsimple = shared_library('simple',
                           'simple.cpp',
                           include_directories : inc,
                           install : true)

# declare_dependency返回一个dependency(.)对象
libsimple_dep = declare_dependency(include_directories : inc,
                                   link_with : libsimple)
```






