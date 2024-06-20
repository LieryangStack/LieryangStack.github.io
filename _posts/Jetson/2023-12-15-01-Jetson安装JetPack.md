---
layout: post
title: 一、Jetson安装JetPack
categories: Jetson
tags: [Jetson]
---

以下操作我都使用的是 `Ubuntu 18.04`，使用 `Ubuntu 20.04` 会出现flash错误。（暂时没有研究为什么出现错误，官网说20.04系统也可以刷机）

## 1 安装 SDK Manager

NVIDIA SDK Manager为主机和目标设备的NVIDIA DRIVE, Jetson, Holoscan, Rivermax, DOCA和以太网交换机SDK提供端到端开发环境设置解决方案。

[下载链接:https://developer.nvidia.com/sdk-manager](https://developer.nvidia.com/sdk-manager)

## 2 Jetson安装系统和相关组件

Orin nx（16G）直接安装的是SSD固态刷系统，如果是Xavier Nx(8G)的非工业版本，需要先插内存卡。

我们先安装JetPack系统，开机后再安装CUDA等相关运行环境，如果一起安装可能会有错误。

### 2.1 Orin Nx(16G)

Orin Nx（16G）系列不支持SD卡烧录，直接安装Nvme固态。

#### 2.1 进入恢复模式安装JetPack

`FC REC`和`GND`引脚短接，如下图所示：

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

我使用的是开发者套件，非emmc版本。可以直接刷SD卡或者Nvme，刷Nvme的时候拔掉内存。配置选择 `runtime` 运行时配置。

我的设备是Jetson Xavier NX，有两个版本，根据实际选择，带EMMC的选P3668-0001 module（商业模组），带SD卡槽的选P3668-0000 module。

![Alt text](/assets/Jetson/01_JetPackInstall/image.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-1.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-2.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-3.png)

![Alt text](/assets/Jetson/01_JetPackInstall/image-4.png)

#### 存在问题

1. NVME问题

By further investigating, the root cause is that the layout used to flash external device is changed to “flash_l4t_t194_nvme.xml” from “flash_l4t_external.xml” in “nvsdkmanager_flash.sh” for T194 device.

This only affects the customer who is using SDKM to flash device on T194 device. For customers who use initrd flash to flash device, there is no effect.

For the customers who want to extend the size of APP partition on external device for T194 device, here is the workaround:

Flashed the device. If customer has flashed device before, skip this step.
Find the “nvsdkmanager_flash.sh” under “~/nvidia/nvidia_sdk//Linux_for_Tegra/” and replace the “flash_l4t_t194_nvme.xml” with “flash_l4t_external.xml”
Re-flash the device

![Alt text](/assets/Jetson/01_JetPackInstall/image-5.png)

2. 无法自动连接虚拟机问题

![Alt text](/assets/Jetson/01_JetPackInstall/image-6.png)

#### 2.2 开机后安装相关组件

