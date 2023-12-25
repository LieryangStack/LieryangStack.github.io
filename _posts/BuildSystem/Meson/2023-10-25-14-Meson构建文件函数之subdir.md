---
layout: post
title: 十四、Meson构建文件函数——subdir()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 subdir()

进入指定的子目录并执行其中的 meson.build 文件。一旦完成，它将返回并继续执行 subdir() 命令之后的下一行。在那个 meson.build 文件中定义的变量随后可用于当前构建文件的后续部分以及使用 subdir() 执行的所有后续构建文件。

请注意，这意味着源代码树中的每个 meson.build 文件只能且必须执行一次。

## 2 subdir()定义

```python
# Enters the specified subdirectory and executes the `meson
void subdir(
  str dir_name,     # Directory relative to the current `meson

  # Keyword arguments:
  if_found : list[dep]  # Only enter the subdir if all dep.found() methods return `true`.
)
```




