---
layout: post
title: 一、WireShark抓包分析——ARP协议
categories: 计算机网络
tags: [计算机网络]
---

## 1 计算机网络模型

首先，回顾一下基础的模型

![Alt text](/assets/ComputerNetwork/2024010901ARP/image.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-2.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-1.png)

## 2 ARP协议基础概念

- arp（Address Resolution Protocol）协议，也称地址解析协议，是根据IP地址获取物理地址的一个TCP/IP协议。它可以解决<font color="red">同一个局域网内</font>主机或路由器的IP地址和MAC地址的映射问题。

- arp协议属于网路层协议。

<font color="red">APR协议是用在一个链路（局域网）里面查询MAC地址，每个设备（比如：电脑）和路由器都会有ARP缓冲</font>

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-4.png)

## 3 ARP协议的流程

![alt text](/assets/ComputerNetwork/2024010901ARP/image-10.png)

ARP高速缓存表

![alt text](/assets/ComputerNetwork/2024010901ARP/image-11.png)

当主机B要给主机C发送数据包时，会首先在自己的ARP高速缓存表中查找主机C的IP地址所对应的MAC地址，但未找到，因此，主机B需要发送ARP请求报文，来获取主机C的MAC地址

![alt text](/assets/ComputerNetwork/2024010901ARP/image-12.png)

ARP请求报文有具体的格式，上图的只是简单描述

ARP请求报文被封装在MAC帧中发送，目的地址为广播地址

主机B发送封装有ARP请求报文的广播帧，总线上的其他主机都能收到该广播帧

![alt text](/assets/ComputerNetwork/2024010901ARP/image-13.png)

收到ARP请求报文的主机A和主机C会把ARP请求报文交给上层的ARP进程

主机A发现所询问的IP地址不是自己的IP地址，因此不用理会

主机C的发现所询问的IP地址是自己的IP地址，需要进行相应

![alt text](/assets/ComputerNetwork/2024010901ARP/image-14.png)

![alt text](/assets/ComputerNetwork/2024010901ARP/image-15.png)

![alt text](/assets/ComputerNetwork/2024010901ARP/image-16.png)

动态与静态的区别

![alt text](/assets/ComputerNetwork/2024010901ARP/image-17.png)

<font color="red">ARP协议只能在一段链路或一个网络上使用，而不能跨网络使用</font>

![alt text](/assets/ComputerNetwork/2024010901ARP/image-18.png)

**总结**

![alt text](/assets/ComputerNetwork/2024010901ARP/image-19.png)

## 3 ARP协议抓包

[分析数据文件：/assets/ComputerNetwork/2024010901ARP/ARP分析.pcapng](/assets/ComputerNetwork/2024010901ARP/ARP分析.pcapng)

### 3.1 手机链接局域网

![Alt text](/assets/ComputerNetwork/2024010901ARP/手机链接局域网.jpg)

**手机：**

- IP地址： `192.168.10.67`

- MAC地址： `B6:EF:91:87:DC:83`

**路由器LAN：**

- IP地址：`192.168.10.1`

- MAC地址：`0C:11:7F:00:B1:94`

**电脑端：**

- IP地址：`192.168.10.67`

- MAC地址：`08:BF:B8:1C:25:D6`

DCHP（路由器的LAN）会先发送一个ARP广播查询信息，查看`192.168.10.67`IP是否有客户使用

#### 3.2 电脑pign 192.168.10.67

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-6.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-7.png)

![Alt text](/assets/ComputerNetwork/2024010901ARP/image-8.png)


## 参考

[参考1：OSI模型中各层单位-报文、报文段、数据报（Datagram）、数据包（Packet）和分组、帧的概念区别](https://blog.csdn.net/dianqicyuyan/article/details/121798895)

