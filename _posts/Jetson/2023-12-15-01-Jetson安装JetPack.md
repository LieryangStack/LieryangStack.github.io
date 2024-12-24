---
layout: post
title: 一、Jetson安装JetPack
categories: Jetson
tags: [Jetson]
---

- 2024年6月：以下操作我都使用的是 `Ubuntu 18.04`，使用 `Ubuntu 20.04` 会出现flash错误。（暂时没有研究为什么出现错误，官网说20.04系统也可以刷机）

- 2024年12月：使用 `Ubuntu 20.04` 均可以正常给 `Xavier Nx` 和 `Orin Nx` 刷机

## 1 安装 SDK Manager

NVIDIA SDK Manager为主机和目标设备的NVIDIA DRIVE, Jetson, Holoscan, Rivermax, DOCA和以太网交换机SDK提供端到端开发环境设置解决方案。

[下载链接:https://developer.nvidia.com/sdk-manager](https://developer.nvidia.com/sdk-manager)

## 2 Jetson安装系统和相关组件

Orin nx（16G）直接安装的是SSD固态刷系统。


我们先安装JetPack系统，开机后再安装CUDA等相关运行环境，如果一起安装可能会有错误。

### 2.1 Orin Nx(16G)

Orin Nx（16G）系列不支持SD卡烧录，直接安装Nvme固态。

#### 2.1 进入恢复模式安装JetPack

`FC REC`和`GND`引脚短接（或者上电前按下恢复按钮），如下图所示：

![Alt text](/assets/Jetson/01_JetPackInstall/短接恢复引脚.png)

电脑主机命令行输入

```sh
# lsusb是显示系统中以及连接到系统的USB总线信息的工具
# NVIDIA Corp. APX就是英伟达设备已经成功连接到主机
lieryang@Galaxy:~$ lsusb
Bus 001 Device 011: ID 0955:7323 NVIDIA Corp. APX
```
&emsp;
![Alt text](/assets/Jetson/01_JetPackInstall/OrinNx系统烧录.png)
其中存储设备必须选择 NVMe，如果选择 SD card 最后会报错，这是因为 Jetson Orin NX 没有 eMMC 存储！！！

这里 `runtime` 和 `pre-config` 两者的区别就是 `pre-config` 是提前配置用户的信息（帐号、密码、地区和语言等）安装在 SDK Manager 所在的系统上的，而 `runtime` 则不提前配置，其实没有太大区别。<span style="color:red;">虽然没有太大区别，但是为了不出问题，最好还是选择 `runtime` 。

### 2.2 Xavier Nx(8G)

我使用的是开发者套件，非emmc版本。可以直接刷SD卡或者Nvme，刷Nvme的时候拔掉内存。配置选择 `runtime` 运行时配置。（Ubuntu20.04直接pre-config没有任何问题）

我的设备是Jetson Xavier NX，有两个版本，根据实际选择，带EMMC的选P3668-0001 module（商业模组），带SD卡槽的选P3668-0000 module。

![Alt text](/assets/Jetson/01_JetPackInstall/image.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-1.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-2.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-3.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-4.png)

## 2 安装过程存在问题总结

### 2.1 无法自动连接虚拟机问题

- 虚拟机（主机）Ubuntu系统版本：Ubuntu18.04

- Jetson设备型号：Jetson Xavier Nx（开发者套件版本）

- 安装JetPack系统版本：任何版本

- 问题描述：等待设备启动过程中（也就是听到风扇全力运行），设备USB不会自动连接虚拟机，需要手动点击虚拟机的右下角USB设备，连接到虚拟机。

![Alt text](/assets/Jetson/01_JetPackInstall/image-6.png)

### 2.2 NVME问题

- 虚拟机（主机）Ubuntu系统版本：Ubuntu20.04

- Jetson设备型号：Jetson Xavier Nx（开发者套件版本）

- 安装JetPack系统版本：5.1.2

- 问题描述：选择NVME为系统安装，会造成固态SSD只能使用16GB，并不是整个固态SSD的内存大小。


By further investigating, the root cause is that the layout used to flash external device is changed to “flash_l4t_t194_nvme.xml” from “flash_l4t_external.xml” in “nvsdkmanager_flash.sh” for T194 device.

This only affects the customer who is using SDKM to flash device on T194 device. For customers who use initrd flash to flash device, there is no effect.

For the customers who want to extend the size of APP partition on external device for T194 device, here is the workaround:

Flashed the device. If customer has flashed device before, skip this step.
Find the “nvsdkmanager_flash.sh” under “~/nvidia/nvidia_sdk//Linux_for_Tegra/” and replace the “flash_l4t_t194_nvme.xml” with “flash_l4t_external.xml”
Re-flash the device

需要将 `flash_l4t_t194_nvme.xml` 替换成 `flash_l4t_external.xml`

- <font color="red">JetPack 5.1.2存在该问题</font>

- <font color="red">JetPack 5.1.4没有存在该问题</font>

![Alt text](/assets/Jetson/01_JetPackInstall/image-5.png)

### 2.3 SSH等待连接问题

- 虚拟机（主机）Ubuntu系统版本：Ubuntu18.04、Ubuntu20.04

- Jetson设备型号：任何型号

- 安装JetPack系统版本：任何版本

- 问题描述：由于我在虚拟机中开启了翻墙VPN，导致连接设备SSH识别

![Alt text](/assets/Jetson/01_JetPackInstall/image-7.png)

虚拟机设置成桥接网络，网络代理设置成主机ip和翻墙端口

![Alt text](/assets/Jetson/01_JetPackInstall/image-8.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-9.png)