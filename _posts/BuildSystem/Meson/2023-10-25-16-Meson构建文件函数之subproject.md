---
layout: post
title: 十六、Meson构建文件函数——subproject()
categories: Meson
tags: [Meson]
---

Meson本质上是用 `Python` 编写的，所以这些函数也都是Python函数。

## 1 subproject()

此功能接受位置参数中指定的项目，并通过返回一个子项目对象将其纳入当前构建规范。子项目必须始终放置在顶级源目录的 subprojects 目录内。例如，名为 foo 的子项目必须位于 `${MESON_SOURCE_ROOT}/subprojects/foo` 中。

default_options（自 0.37.0 起）：一个默认选项值数组，这些值覆盖子项目中的 meson.options 设置的选项值（如 project 中的 default_options，它们仅在首次运行 Meson 时生效，命令行参数会覆盖构建文件中的任何默认选项）。（自 0.54.0 起）：也可以覆盖 default_library 内置选项。（自 1.2.0 起）：可以传递字典而不是数组。
version：其工作方式与 dependency 中的相同。它指定子项目应该是的版本，例如 >=1.0.1。
required（自 0.48.0 起）：默认情况下，required 为 true，如果无法设置子项目，Meson 将中止。您可以将此设置为 false，然后使用子项目对象上的 .found() 方法。您也可以传递与 dependency() 相同的特性选项的值。

请注意，您可以使用返回的子项目对象访问子项目中的任何变量。但是，如果您想在子项目内部使用依赖项对象，更简单的方法是使用 dependency() 的 fallback: 关键字参数。

## 2 subproject()

```python
# Takes the project specified in the positional argument and brings that
subproject subproject(
  str subproject_name,     # Name of the subproject

  # Keyword arguments:
  default_options : list[str] | dict[str | bool | int | list[str]]  # An array of default option values
  required        : bool | feature                                # Works just the same as in dependency().
  version         : str                                           # Works just like the same as in dependency().
)
```




