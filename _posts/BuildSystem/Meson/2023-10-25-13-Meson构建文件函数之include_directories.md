---
layout: post
title: 十三、Meson构建文件函数——include_directories()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 include_directories()

- 仅仅调用该函数，并不会把目录添加到搜索路径中，因为meson不像cmake不具有全局搜索路径。

- 给出的每个目录都被转换为<font color="red">两个包含路径</font>：一个相对于源代码根目录，另一个相对于构建根目录。(所以可以不提供绝对路径，我们既可以获取构建目录路径，也同时获取源代码根目录，源代码根目录就是meson.build所在目录)

- `@includes`变量可以同时传入多个目录。

## 2 include_directories()定义

```python
'''
Brief: 
      根据传入的目录，返回一个不透明对象
Param:
      includes(str): 接受0到多个字符串类型路径
      
      is_system(bool 默认=flase): 如果设置为 true，则将指定的目录标记为系统目录。
      这意味着在支持该标志的编译器上，它们将与 -isystem 编译器参数一起使用，
      而不是 -I（实际上，除了 Visual Studio 以外的所有编译器都支持该标志）。

Returns:

'''

# param includes: 接受 0 到无限个类型为 str 的可变参数


inc include_directories(
  str includes...,  # Include paths to add

  # Keyword arguments:
  is_system : bool  # If set to `true`, flags the specified directories as system directories
)
```



## 3 include_directories()举例

### 3.1 示例一

可以同时传入多个目录，此时会返回四个路径，<font color="red">系统会检查保留存在的路径</font>，不存在的路径就不会保留。

- /project_path

- /project_path/gst/rtsp-server

- /project_path/build_path

- /project_path/build_path/rtsp-server

```python
rtspserver_incs = include_directories('gst/rtsp-server', '.')
```

## 参考

[官网：include_directories()
](https://mesonbuild.com/Reference-manual_functions.html#include_directories)
