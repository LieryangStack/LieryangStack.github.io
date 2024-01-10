---
layout: post
title: 一、WireShark分析——ARP协议
categories: 计算机网络
tags: [计算机网络]
---

## 1 计算机网络模型

首先，回顾一下基础的模型

![Alt text](/assets/ComputerNetwork/2024010901ARP/image.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-2.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-1.png)

数据单元（data unit）：指许多信息单元。常用的数据单元有服务数据单元（SDU）、协议数据单元（PDU）。SDU是在同一机器上的两层之间传送信息。PDU是发送机器上每层的信息发送到接收机器上的相应层（同等层间交流用的）。

1. 每一层对数据的称呼是不一样的，都是本质上都是数据。 <font color=#ff0000>下面对特殊和没有提到的情况说明：</font>

    数据报（Datagram）：面向无连接的数据传输（UDP），其工作过程类似于报文交换。采用数据报方式传输时，被传输的分组称为数据报。
    有的书为了把OSI中的传输层里的TCP和UDP区别开来，将上层传下来的数据（也叫数据流）进行分段。用TCP的就叫报文段，用UDP的就叫用户数据报，亦可称它们为数据段

    数据单元（data unit）：指许多信息单元。常用的数据单元有服务数据单元（SDU）、协议数据单元（PDU）。SDU是在同一机器上的两层之间传送信息。PDU是发送机器上每层的信息发送到接收机器上的相应层（同等层间交流用的）。

2. OSI模型传输层和网络层可以是任意协议，但是TCP/IP模型，网络层是IP协议，传输层是TCP协议。

## 2 ARP协议基础概念

arp协议，也称地址解析协议，是根据IP地址获取物理地址的一个TCP/IP协议。它可以解决同一个局域网内主机或路由器的IP地址和MAC地址的映射问题。arp协议在TCP/IP模型中属于IP层（网络层），在OSI模型中属于链路层。

<font color=#ff0000>APR协议是用在一个链路（局域网）里面查询MAC地址，每个设备（比如：电脑）和路由器都会有ARP缓冲</font>

我的理解：比如数据从A链路通过路由A发送到B链路的路由B，B链路里面的路由会根据目标MAC发送到相应端口。（MAC地址是在链路层使用，也就是一个链路里面传输使用MAC地址）

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-4.png)

## 3 ARP协议抓包

[分析数据文件：/assets/ComputerNetwork/2024010901ARP/ARP分析.pcapng](/assets/ComputerNetwork/2024010901ARP/ARP分析.pcapng)

### 3.1 手机链接局域网

![Alt text](手机链接局域网.jpg)

- IP地址： `192.168.10.67`
- MAC地址： `B6:EF:91:87:DC:83`

DCHP应该会先发送一个ARP查询信息，查看`192.168.10.67`IP是否有客户使用

#### 3.2 电脑pign 192.168.10.67

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-6.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-7.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-8.png)


## 参考

[参考1：OSI模型中各层单位-报文、报文段、数据报（Datagram）、数据包（Packet）和分组、帧的概念区别](https://blog.csdn.net/dianqicyuyan/article/details/121798895)