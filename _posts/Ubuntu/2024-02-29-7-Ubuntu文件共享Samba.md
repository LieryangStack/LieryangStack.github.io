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

### 1.2 给Samba用户设置密码

```sh
# -a 表示添加smbpasswd用户
sudo smbpasswd -a lieryang
```

### 1.3 修改配置文件

```sh
# 为保险起见，先备份一下原来的 Samba 配置文件。
sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak
# 编辑smb.conf配置文件，添加共享目录
sudo gedit /etc/samba/smb.conf
```

根据共享目录和系统用户添加以下配置信息

```sh
[lieryang]
  path = /home/lieryang
  public = yes
  writable = yes
  available = yes
  browseable = yes
  valid users = lieryang
```

### 1.4 重启Samba服务器

```sh
systemctl restart smbd.service
systemctl enable smbd.service
```