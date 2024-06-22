---
layout: post
title: 十八、Meson构建文件函数——dependency笔记总结版本
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 dependency()

- dependency 用于查找给定名称的外部依赖项（通常是系统上安装的库），首先通过 pkg-config 查找，如果失败则通过 CMake。如果查找失败，可以使用[fallback回退处理](https://mesonbuild.com/Dependencies.html#dependencies-with-custom-lookup-functionality)，支持 `pkg-config`、`cmake`、`extraframework`(OSX only) 、 `qmake` 等。

- dependency 返回的是 `dep` 对象，该对象一共有以下六种方式创建：
  
  ![alt text](/assets/BuildSystem/Meson/18_Dependency/image/image-1.png)


## 2 dependency()定义

```python
dep dependency(
  str names...,  # The names of the dependency to look up

  # 指定 Meson 是否应自动选择fallback子项目
  allow_fallback    : bool
  # 默认选项值的数组
  default_options   : list[str] | dict[str | bool | int | list[str]]
  # 返回 disabler() 对象，而不是未找到的依赖项 default = false
  disabler          : bool
  # 手动指定一个fallback子项目（该部分在声明依赖declare_dependency中讲）
  fallback          : list[str] | str
  # 一个枚举标志，标记依赖项的包含类型 default = 'preserve'，可以转换成 -isystem 类型头文件搜索路径
  include_type      : str 
  # 定义要查找的特定语言依赖项
  language          : str
  # 定义检测依赖项的方式，默认是 default = 'auto'
  method            : str
  # 如果设置为 `true`，则 Meson 会在本机系统上查找依赖项
  native            : bool
  # 一个可选的字符串，如果未找到依赖项，将作为消息打印
  not_found_message : str 
  # 当设置为 `false` 时，Meson 将继续构建
  required          : bool | feature
  # 告诉依赖项提供者尝试获取静态依赖项
  static            : bool
  version           : list[str] | str
```

## 3 dependency查找依赖项

- 可以设定`@method`变量来设定查找依赖项方式，其中包括：pkg-config、config-tool、cmake、builtin、system、sysconfig、qmake、extraframework 和 dub。

- 如果不指定默认值是 `auto`。auto 的依赖项方法顺序为：

  1. pkg-config

  2. cmake
  
  3. extraframework（仅限 OSX）

### 3.1 查找依赖项示例

```python
qt5_dep = dependency('qt5', modules : ['Core', 'Gui'], method : 'config-tool')
gstreamer_dep        = dependency('gstreamer-1.0', method: 'pkg-config')
gstreamer_video_dep  = dependency('gstreamer-video-1.0', version: '>=1.30')
```

### 3.2 获取依赖项pkgconfig文件中的变量

我们可以获取 `gstreamer-1.0.pc` 文件中的变量，通过 `dep` 对象的 `get_pkgconfig_variable` 函数。

```python
'''
Brief: 
      dependency创建了dep对象，该对象可以调用该函数，返回pkg-config文件中@var_name变量的值。
Argument:
      var_name(str): pkg-config文件中的变量，例如：prefix、includedir、libdir等
      default(str): 如在文件中找不到@var_name变量，则返回@default值
      define_variable: 重定义变量的值，格式如下 ['prefix', 'tmp', 'libdir', '/usr/local/gsreamer-1.22.6']

'''
str get_pkgconfig_variable(
  str var_name,
  default         : str
  define_variable : list[str]
)
```

下面图片表示获取 `libdir` 变量路径，同时修改 `prefix` 变量

![alt text](/assets/BuildSystem/Meson/18_Dependency/image/image.png)

也可以使用 `get_variable` 函数，获取其他方式 `method` 中配置文件的值。例如：

`var = foo_dep.get_variable(cmake : 'CMAKE_VAR', default_value : 'default')`


### 3.3 获取CUDA依赖

```python
dep = dependency('cuda', version : '>=10.0', modules : ['cublas'])
```

这里的组件就是库的名称，比如：

- `['cudart']` 指的就是 `libcudart.so` 

- `['cudnn']` 指的就是 `libcudnn.so`

如果没有找到CUDA，可以使用环境变量 `CUDA_PATH` 显式指定CUDA路径。


### 3.4 通过得到的变量获取该路径的其他文件

有时依赖项提供可安装的文件，其他项目需要使用。例如，wayland-protocols 的 XML 文件。

```python
foo_dep = dependency('foo')
foo_datadir = foo_dep.get_variable('pkgdatadir')
custom_target(
    'foo-generated.c',
    input: foo_datadir / 'prototype.xml',
    output: 'foo-generated.c',
    command: [generator, '@INPUT@', '@OUTPUT@']
)
```

## 4 使用 compiler.find_library 创建dep对象

```python
project('myproj', 'c')

cc = meson.get_compiler('c')
lib_hello = cc.find_library('hello',
               dirs : ['/opt/hello'])
inc_hello = include_directories('/opt/hello')
exec = executable('app',
                  'main.c',
                  dependencies : [lib_hello],
                  include_directories : inc_hello)
```

## 参考

[参考1：Dependencies介绍](https://mesonbuild.com/Dependencies.html#dependencies-with-custom-lookup-functionality)


[参考1：Dependency函数](https://mesonbuild.com/Reference-manual_functions.html#dependency)

[参考2：Dependency object(dep对象)](https://mesonbuild.com/Reference-manual_returned_dep.html#depget_pkgconfig_variable)

