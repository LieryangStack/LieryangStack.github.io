---
layout: post
title: 十三、Meson构建文件函数——include_directories()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 include_directories()

返回一个不透明对象，其中包含位置参数中给出的相对于当前目录的目录。然后可以将结果传递给构建可执行文件或库时的 include_directories: 关键字参数。您可以在任何子目录中使用返回的对象，Meson 会自动使路径正常工作。

请注意，此函数调用本身不会将目录添加到搜索路径中，因为没有全局搜索路径。对于类似的功能，请参阅 add_project_arguments()。

另请参阅 executable() 的 implicit_include_directories 参数，它将当前源代码和构建目录添加到包含路径中。

给出的每个目录都被转换为两个包含路径：一个相对于源代码根目录，另一个相对于构建根目录。

## 2 include_directories()定义

```python
# Returns an opaque object which contains the directories (relative to
inc include_directories(
  str includes...,  # Include paths to add

  # Keyword arguments:
  is_system : bool  # If set to `true`, flags the specified directories as system directories
)
```

## 3 include_directories()举例

### 3.1 示例一


```python

```

### 3.2 示例二

```python

```

