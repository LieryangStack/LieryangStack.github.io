---
layout: post
title: 七、Ubuntu文件共享samba
categories: Ubuntu
tags: [Ubuntu]
---

Samba是一个在Unix、Linux和其他类Unix系统上运行的软件，用于实现这些系统与Windows系统之间的文件和打印服务共享。它允许基于SMB/CIFS协议的文件和打印服务共享，这是Windows系统用于网络文件和打印服务共享的标准协议。

<font color=red>Samba可以使得Windows、Linux和手机之间共享查看文件</font>

## 1 Ubuntu开启文件共享

### 1.1 安装Samba服务器
```sh
sudo apt-get install -y samba samba-common
```

### 1.2 
