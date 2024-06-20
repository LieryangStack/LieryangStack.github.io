---
layout: post
title: 二十、Meson构建文件函数——configuration_data()
categories: Meson
tags: [Meson]
---

## 如果有输入文件

输入文件
```
#mesondefine TOKEN
```

替换操作取决于 TOKEN 的值和类型：

```python
#define TOKEN     // 如果 TOKEN 设置为布尔值 true
#undef TOKEN      // 如果 TOKEN 设置为布尔值 false
#define TOKEN 4   // 如果 TOKEN 设置为整数或字符串值
/* undef TOKEN */ // 如果 TOKEN 未设置任何值

```

## 如果没有输入文件
```c
conf_data.set('FOO', '"string"') => #define FOO "string"
conf_data.set('FOO', 'a_token')  => #define FOO a_token
conf_data.set('FOO', true)       => #define FOO
conf_data.set('FOO', false)      => #undef FOO
conf_data.set('FOO', 1)          => #define FOO 1
conf_data.set('FOO', 0)          => #define FOO 0
```