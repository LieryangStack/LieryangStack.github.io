---
layout: post
title: C_Plus_Plus关于命名空间知识点
subtitle: 
categories: 日之漫录
tags: [日之漫录]
---

## 1 命名空间内实现
```c++
namespace NsOne {
  int a = 1;
  int func () {
    std::cout << a << std::endl;
    return 0;
  }
}
```
## 2 命名空间内定义，外部实现
```c++
namespace NsTwo {
  extern int var;
  void func ();
  class Hi;
}
```

```c++
int NsTwo::var = 2;

class NsTwo::Hi {
public:
  /* C++11之后是允许的，在成员变量声明的同时进行初始化 */
  int a = 1;
};

void
NsTwo::func () {
  std::cout << NsTwo::var << std::endl;
}
```

## 3 命名空间的合并
```c++
/* source1.h */
namespace NsThree {
  int a = 3;
}
```

```c++
/* source2.h */
namespace NsThree {
  extern int a; /* 可以不去声明 */
  void func() {
    std::cout << a << std::endl;
  }
}
```

该项目测试文件在 [/assets/日之漫录/20230829](/assets/rizhimanlu/20230829)

## 参考
[一、C++在命名空间中声明类和成员函数](https://blog.csdn.net/ningmengccccc/article/details/80791772)

[二、C++小知识-名字空间的声明、定义与合并](https://blog.csdn.net/rong11417/article/details/106462434)