---
layout: post
title: 十九、Meson构建文件函数——declare_dependency()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 declare_dependency()

这个函数返回一个 dep 对象，其行为类似于 dependency() 的返回值，但是仅在当前构建内部使用。这在子项目中非常有用。它允许子项目轻松指定如何被使用。这使得它可以与系统外部提供的同一依赖项互换。

## 2 declare_dependency()定义

```python
# This function returns a dep object that
dep declare_dependency(
  compile_args        : list[str]                                                    # Compile arguments to use
  d_import_dirs       : list[inc | str]                                              # the directories to add to the string search path (i
  d_module_versions   : str | int | list[str | int]                                  # The [D version identifiers](https://dlang
  dependencies        : list[dep]                                                    # Other dependencies needed to use this dependency
  extra_files         : list[str | file]                                             # extra files to add to targets
  include_directories : list[inc | str]                                              # the directories to add to header search path,
  link_args           : list[str]                                                    # Link arguments to use
  link_whole          : list[lib]                                                    # Libraries to link fully, same as executable().
  link_with           : list[lib]                                                    # Libraries to link against
  objects             : list[extracted_obj]                                          # a list of object files, to be linked directly into the targets that use the
  sources             : list[str | file | custom_tgt | custom_idx | generated_list]  # sources to add to targets
  variables           : dict[str] | list[str]                                        # a dictionary of arbitrary strings,
  version             : str                                                          # the version of this dependency,
)
```

### 3 示例

#### 3.1 示例1

直接使用动态库

```python
# Up to you to select which .so file
foo_lib = 'lib/lin64/foo.so'
foo_dep = declare_dependency(link_with: foo_lib)
install_data(foo_lib, install_dir : get_option('libdir'))
```

#### 3.2 示例2

使用编译好的静态库

```python
my_inc = include_directories(...)
my_lib = static_library(...)
my_dep = declare_dependency(link_with : my_lib,
                            include_directories : my_inc)
```