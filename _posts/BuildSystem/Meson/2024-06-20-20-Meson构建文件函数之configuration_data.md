---
layout: post
title: 二十、Meson构建文件函数——configuration_data()
categories: Meson
tags: [Meson]
---

通过在构建文件设置配置信息，生成头文件，例如`config.h`

## 1 如果有输入文件

输入文件名为 `config.h.in，内容如下：

```
#define VERSION_STR "@version@"

#mesondefine TOKEN
```

`#mesondefine TOKEN`可以替换内容如下：

```c
#define TOKEN     // 如果 conf_data.set('TOKEN', true)
#undef TOKEN      // 如果 conf_data.set('TOKEN', false)
#define TOKEN 4   // 如果 conf_data.set('TOKEN', 4)
#define TOKEN "token" // 如果 conf_data.set('TOKEN', '"token"')
/* undef TOKEN */ // 如果 TOKEN 未设置任何值
```

`meson.build`文件内容如下(替换操作取决于 TOKEN 的值和类型)：

```python
project('my_project', 'cpp', version: '1.0')

conf_data = configuration_data()
conf_data.set('version', '1.2.3') 
conf_data.set('TOKEN', '"token"')

# 因为没有 `mesondefine BAR` 所以不会有该定义
conf_data.set('BAR', true, description : 'Set BAR if it is available')

# 有输入配置文件
configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf_data)
```


## 2 如果没有输入文件

配置文件到头文件对应如下：

```c
conf_data.set('FOO', '"string"') => #define FOO "string"
conf_data.set('FOO', 'a_token')  => #define FOO a_token
conf_data.set('FOO', true)       => #define FOO
conf_data.set('FOO', false)      => #undef FOO
conf_data.set('FOO', 1)          => #define FOO 1
conf_data.set('FOO', 0)          => #define FOO 0
```

`config1.h` 和 `config2.h` 都是没有输入配置文件

```python
project('my_project', 'cpp', version: '1.0')

conf_data = configuration_data()
conf_data.set('version', '1.2.3')
conf_data.set('TOKEN', '"token"')
conf_data.set('BAR', true, description : 'Set BAR if it is available')

# 有输入配置文件
configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf_data)

configure_file(output: 'config1.h',
               configuration: conf_data)


configure_file(output : 'config2.h',
  configuration : {
    'STRING' : '"foo"',
    'INT' : 42,
    'DEFINED' : true,
    'UNDEFINED' : false,
  }
)

executable('demo', 'main.cpp')
```