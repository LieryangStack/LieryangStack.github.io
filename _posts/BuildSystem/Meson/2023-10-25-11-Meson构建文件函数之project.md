---
layout: post
title: 十一、Meson构建文件函数——project()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 project()

`project()` 每个项目中调用的第一个函数，用于初始化 Meson。

此函数的第一个参数必须是一个字符串，用于定义该项目的名称。

项目名称可以是您想要的任何字符串，它只用于描述目的。然而，由于它被写入，例如依赖性清单，通常有意义的是将其与项目压缩包或 pkg-config 名称保持一致。因此，例如，你可能希望使用 libfoobar 而不是 The Foobar Library 作为名称。

其后可以跟随项目使用的编程语言列表。（自 0.40.0 版本起）语言列表是可选的。

这些语言可用于 native: false（默认值）（host机器）目标和 native: true（build机器）目标。（自 0.56.0 版本起）不需要指定语言的构建机器编译器。

支持的语言值包括 c、cpp（用于 C++）、cuda、cython、d、objc、objcpp、fortran、java、cs（用于 C#）、vala 和 rust。

## 2 project()定义

```python
# The first function called in each project, to initialize Meson
void project(
  str project_name,     # The name of the project
  str language...,      # The languages that Meson should initialize

  # Keyword arguments:
  default_options : list[str] | dict[str | bool | int | list[str]]  # Accepts strings in the form `key=value`
  license         : str | list[str]                               # Takes a string or array of strings describing the license(s) the code is under
  license_files   : str | list[str]                               # Takes a string or array of strings with the paths to the license file(s)
  meson_version   : str                                           # Takes a string describing which Meson version the project requires
  subproject_dir  : str                                           # Specifies the top level directory name that holds Meson subprojects
  version         : str | file                                    # A free form string describing the version of this project
)
```

## 3 project()举例

```meson
project('glib', 'c',
  version : '2.76.6',
  # NOTE: See the policy in docs/meson-version.md before changing the Meson dependency
  meson_version : '>= 0.60.0',
  default_options : [
    'buildtype=debugoptimized',
    'warning_level=3',
    'c_std=gnu99'
  ]
)
```
