---
layout: post
title: 十四、Meson构建文件——get_option()
categories: Meson
tags: [Meson]
---

大多数非平凡的构建都需要用户可设置的选项。例如，一个程序可能有两种不同的数据后端，可以在构建时选择。Meson 通过拥有一个选项定义文件来实现这一点。其名称为 meson.options，放置在源代码树的根目录。在 1.1 版本之前的 Meson 版本中，这个文件被称为 meson_options.txt。

这是一个简单的选项文件。

```python
option('someoption', type : 'string', value : 'optval', description : 'An option')
option('other_one', type : 'boolean', value : false)
option('combo_opt', type : 'combo', choices : ['one', 'two', 'three'], value : 'three')
option('integer_opt', type : 'integer', min : 0, max : 5, value : 3) # 自 0.45.0 版本起
option('free_array_opt', type : 'array', value : ['one', 'two']) # 自 0.44.0 版本起
option('array_opt', type : 'array', choices : ['one', 'two', 'three'], value : ['one', 'two'])
option('some_feature', type : 'feature', value : 'enabled') # 自 0.47.0 版本起
option('long_desc', type : 'string', value : 'optval',
description : 'An option with a very long description' +
'that does something in a specific context') # 自 0.55.0 版本起
有关内置选项，请参阅内置选项。
```
## 1 构建选项类型

所有类型都允许设置描述值来描述选项，如果没有设置描述，则会使用选项的名称代替。

### 1.1 Strings

字符串类型是自由格式的字符串。如果没有设置默认值，则将使用空字符串作为默认值。

### 1.2 Booleans

布尔值可能是 true 或 false。如果没有提供默认值，则将使用 true 作为默认值。

### 1.3 Combos

组合框允许选择 choices 参数中的任何一个值。如果没有设置默认值，则第一个值将是默认值。

### 1.4 Integers

整数选项包含一个单独的整数，可选的上限和下限值由 min 和 max 关键字参数指定。

此类型自 Meson 版本 0.45.0 起可用。

### 1.5 Arrays

数组表示字符串数组。默认情况下，数组可以包含任意字符串。要限制可以使用的可能值，请设置 choices 参数。Meson 将仅允许值数组包含给定列表中的字符串。数组可以为空。value 参数指定选项的默认值，如果未设置，则将使用 choices 的值作为默认值。

从 0.47.0 版本起，-Dopt= 和 -Dopt=[] 都传递一个空列表，此前 -Dopt= 会传递一个带有空字符串的列表。

此类型自 0.44.0 版本起可用。

### 1.6 Features

特性选项有三种状态：enabled、disabled 或 auto。它旨在作为大多数函数的 required 关键字参数的值传递。目前在 add_languages()、compiler.find_library()、compiler.has_header()、dependency()、find_program()、import() 和 subproject() 函数中支持。

- **enabled**： 等同于传递 required : true。
- **auto**： 等同于传递 required : false。
- **disabled**： 不查找依赖项并始终返回“未找到”。

使用 get_option() 获取此类型选项的值时，会返回一个特殊的特性对象，而不是选项值的字符串表示。此对象可以传递给 required：

```python
d = dependency('foo', required : get_option('myfeature'))
if d.found()
  app = executable('myapp', 'main.c', dependencies : [d])
endif
```

要检查特性features的值，对象有三个返回布尔值且不带参数的方法：

- **.enabled()**
- **.disabled()**
- **.auto()**
这对于根据特性进行自定义代码很有用：

```python
# 检查特性是否不是enable
if get_option('myfeature').enabled()
  # ...
endif
```

如果特性选项的值设置为 auto，则该值将被全局 auto_features 选项（默认为 auto）覆盖。这旨在供打包者使用，他们希望完全控制哪些依赖项是必需的，哪些是禁用的，而不是依赖于构建依赖项安装（在正确的版本）来启用特性。他们可以将 `auto_features=enabled` 以启用所有特性，并仅显式禁用他们不想要的少数特性（如果有的话）。

此类型自 0.47.0 版本起可用。

## 2 弃用的选项

自 0.60.0 版本起

项目选项可以标记为弃用，当用户设置值时 Meson 将发出警告。也可以只弃用某些选择，并将弃用的值映射到新值。

```python
# 选项完全弃用，设置任何值时都会发出警告。
option('o1', type: 'boolean', deprecated: true)

# 其中一个选择被弃用，仅当列表中的值为 'a' 时发出警告。
option('o2', type: 'array', choices: ['a', 'b'], deprecated: ['a'])

# 其中一个选择被弃用，仅当列表中的值为 'a' 时发出警告并将其替换为 'c'。
option('o3', type: 'array', choices: ['a', 'b', 'c'], deprecated: {'a': 'c'})

# 布尔选项被特性替代，旧的 true/false 值被重新映射。
option('o4', type: 'feature', deprecated: {'true': 'enabled', 'false': 'disabled'})

# 特性选项被布尔值替代，启用/禁用/auto 值被重新映射。
option('o5', type: 'boolean', deprecated: {'enabled': 'true', 'disabled': 'false', 'auto': 'false'})
```

自 0.63.0 版本起，deprecated 关键字参数可以接受替换此选项的新选项的名称。在这种情况下，对弃用选项设置值将同时设置旧名和新名的值，假设它们接受相同的值。

```python
# 一个布尔选项被另一个名字的特性替代，旧的 true/false 值，被新选项接受，以实现向后兼容。
option('o6', type: 'boolean', value: 'true', deprecated: 'o7')
option('o7', type: 'feature', value: 'enabled', deprecated: {'true': 'enabled', 'false': 'disabled'})

# 一个项目选项被模块选项替代
option('o8', type: 'string', value: '', deprecated: 'python.platlibdir')
```

## 3 使用构建选项

```python
optval = get_option('opt_name')
```

这个函数还允许你查询 Meson 的内置项目选项的值。例如，要获取安装前缀，你可以使用以下命令：

```python
prefix = get_option('prefix')
```
应当注意的是，你不能在你的 Meson 脚本中设置选项值。它们必须通过 meson configure 命令行工具在外部设置。在构建目录中运行没有参数的 meson configure 会显示你可以设置的所有选项。

要更改它们的值，请使用 -D 选项：

```python
$ meson configure -Doption=newvalue
```

设置数组值有点特殊。如果你只传递一个字符串，则认为它包含所有用逗号分隔的值。因此，调用以下命令：

```python
$ meson configure -Darray_opt=foo,bar
```

将把值设置为两个元素的数组，foo 和 bar。

如果你需要在字符串值中包含逗号，则需要像这样使用适当的 shell 引用传递值：

```python
$ meson configure "-Doption=['a,b', 'c,d']"
```

内部值必须始终是单引号，外部是双引号。

要更改子项目中的值，请在子项目的名称前加上冒号：

```python
$ meson configure -Dsubproject:option=newvalue
```

注意：如果你不能调用 meson configure，你可能使用的是 Meson 的旧版本。在这种情况下，你可以调用 mesonconf，但在新版本中这已经不推荐使用了。

## 4 Yielding to superproject option

假设你有一个主项目和一个子项目。在某些情况下，使两者具有相同的选项值可能是有用的。这可以通过 yield 关键字实现。假设你有这样一个选项定义：

```python
option('some_option', type : 'string', value : 'value', yield : true)
```

如果你单独构建这个项目，这个选项的行为就像平常一样。然而，如果你将这个项目作为另一个项目的子项目来构建，这个项目也有一个名为 some_option 的选项，那么调用 get_option 将返回超级项目的值。如果 yield 的值为 false，get_option 返回子项目选项的值。

## 5 内置的构建选项

有许多内置选项。其实就是 `meson setup` 或者 `meson` 命令的选项参数。例如：`prefix`，`buildtype`等等。

可以通过以下默认选项，设定这些内置选项的参数。

### 5.1 通过project()设定内置选项

```python
project('my_project', 'c', default_options: ['prefix=/home/lieryang/Desktop'])
executable('my_exe', ...)
```

### 5.2 通过命令行设定内置选项

所有这些都可以通过向 meson（也就是 meson setup）传递 `-Doption=value` 来设置，或者在您的 meson.build 中的 project() 的 default_options 内设置。某些选项也可以通过 --option=value 或 --option value 设置。

```sh
# 内置选项可以使用 --option=value，用户编写的选项必须使用 -Doption=value
meson setup build --prefix=/home/lieryang/Desktop
## 或者
meson setup build -Dprefix=/home/lieryang/Desktop
```

## 6 get_option()

获取位置参数中指定的项目构建选项的值。

请注意，对于以 dir 结尾的内置选项（如 bindir 和 libdir）返回的值通常是相对于（并位于）前缀的路径，但您不应依赖于此，因为在某些情况下它也可以是绝对路径。install_dir 参数按预期处理这一点，但如果您需要绝对路径，例如用于定义等，您应该使用路径连接运算符，如此：get_option('prefix') / get_option('localstatedir')。永远不要手动将路径拼接为字符串。

对于类型为 feature 的选项，返回的是一个特性选项对象，而不是字符串。有关更多详细信息，请参阅特性选项文档。

### 6.1 get_option()定义

```python
# Obtains the value of the [project build option](Build-options
str | int | bool | feature | list[str | int | bool] get_option(
  str option_name,     # Name of the option to query
)
```

## 7 示例程序

[测试程序文件夹目录：/assets/BuildSystem/Meson/14_WriteMesonFile/](/assets/BuildSystem/Meson/14_WriteMesonFile/)

![Alt text](/assets/BuildSystem/Meson/14_WriteMesonFile/测试选项和重新配置.png)

## 8 参考

[参考1：Build options](https://mesonbuild.com/Build-options.html#builtin-build-options)
[参考2：Built-in options](https://mesonbuild.com/Builtin-options.html)

