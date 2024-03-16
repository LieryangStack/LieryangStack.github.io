---
layout: post
title: 三、WireShark抓包分析——HTTP协议
categories: 计算机网络
tags: [计算机网络]
---

## 1 TCP报文格式

![Alt text](/assets/ComputerNetwork/2024010903TCP/image-5.png)
![Alt text](/assets/ComputerNetwork/2024010903TCP/image-6.png)

### 1.1 TCP选项

常见的TCP选项有7种，如图所示：

![Alt text](/assets/ComputerNetwork/2024010903TCP/image-7.png)

​1、kind=0，选项表结束（EOP）选项

一个报文段仅用一次。放在末尾用于填充，用途是说明：首部已经没有更多的消息，应用数据在下一个32位字开始处。

2、kind=1，空操作（NOP）选项

没有特殊含义，一般用于将TCP选项的总长度填充为4字节的整数倍。

3、kind=2，最大报文段长度（MSS）选项

TCP连接初始化时，通信双方使用该选项来协商最大报文段长度。<font color='red'>TCP模块通常将MSS设置为（MTU减去40）字节（减掉的这40字节包括20字节的TCP头部和20字节的IP头部）。这样携带TCP报文段的IP数据报的长度就不会超过MTU（假设TCP头部和IP头部都不包含选项字段，并且这也是一般情况），从而避免本机发生IP分片。对以太网而言，MSS值是1460（1500-40）字节。</font>

>MTU指的是在一个网络层（如以太网、Wi-Fi）上可以传输的最大数据包的大小。

4、kind=3，窗口扩大因子选项

TCP连接初始化时，通信双方使用该选项来协商接收窗口的扩大因子。在TCP的头部中，接收窗口大小是用16位表示的，故最大为65535字节，但实际上TCP模块允许的接收窗口大小远不止这个数（为了提高TCP通信的吞吐量）。窗口扩大因子解决了这个问题。

假设TCP头部中的接收通告窗口大小是N，窗口扩大因子（移位数）是M，那么TCP报文段的实际接收通告窗口大小是N*(2^M)，或者说N左移M位。注意，M的取值范围是0～14。我们可以通过修改/proc/sys/net/ipv4/tcp_window_scaling内核变量来启用或关闭窗口扩大因子选项。

和MSS选项一样，窗口扩大因子选项只能出现在同步报文段中，否则将被忽略。但同步报文段本身不执行窗口扩大操作，即同步报文段头部的接收窗口大小就是该TCP报文段的实际接收窗口大小。当连接建立好之后，每个数据传输方向的窗口扩大因子就固定不变了。

5、kind=4，选择性确认（SelectiveAcknowledgment，SACK）选项

TCP通信时，如果某个TCP报文段丢失，则TCP会重传最后被确认的TCP报文段后续的所有报文段，这样原先已经正确传输的TCP报文段也可能重复发送，从而降低了TCP性能。SACK技术正是为改善这种情况而产生的，它使TCP只重新发送丢失的TCP报文段，而不用发送所有未被确认的TCP报文段。选择性确认选项用在连接初始化时，表示是否支持SACK技术。我们可以通过修改/proc/sys/net/ipv4/tcp_sack 内核变量来启用或关闭选择性确认选项。

6、kind=5，SACK实际工作的选项

该选项的参数告诉发送方本端已经收到并缓存的不连续的数据块，从而让发送端可以据此检查并重发丢失的数据块。每个块边沿（edgeofblock）参数包含一个4字节的序号。其中块左边沿表示不连续块的第一个数据的序号，而块右边沿则表示不连续块的最后一个数据的序号的下一个序号。这样一对参数（块左边沿和块右边沿）之间的数据是没有收到的。因为一个块信息占用8字节，所以TCP头部选项中实际上最多可以包含4个这样的不连续数据块（考虑选项类型和长度占用的2字节）。

7、kind=8，时间戳选项

该选项提供了较为准确的计算通信双方之间的回路时间（RoundTrip Time，RTT）的方法，从而为TCP流量控制提供重要信息。我们可以通过修改/proc/sys/net/ipv4/tcp_timestamps内核变量来启用或关闭时间戳选项。

## 2 HTTP分析

### 2.1 TCP三次握手

[实验文件：/assets/ComputerNetwork/2024010903TCP/HTTP_Capture_Test.pcapng](/assets/ComputerNetwork/2024010903TCP/HTTP_Capture_Test.pcapng)

本次使用HTTP协议学习章节中搭建的服务器，通过访问服务器`127.0.0.1:80`，抓包数据分析。

![Alt text](/assets/ComputerNetwork/2024010903TCP/image.png)

打开WireShark首选项关闭分析TCP序列号

![Alt text](/assets/ComputerNetwork/2024010903TCP/image-1.png)

可以看到第一次发送握手信号的时候，TCP报文会有选项表示MSS和窗口扩大因子。此时首部长度（TCP格式前部长度32字节）

传输数据的时候没有TCP选项，所以TCP首部长度20字节

![Alt text](/assets/ComputerNetwork/2024010903TCP/image-2.png)

### 2.2 TCP数据传输

<font color='red'>注意：</font>

1. 完成三次握手的时候Flags依次是 `[SYN] [SYN,ACK] [ACK]`,客户端或者服务器传输数据的时候Flags是 `[PSH, ACK]`。
2. 传输数据的时候：发送方`ACK = Seq(接受方）`，接收方 `ACK = Seq(发送方) + Len(有效报文长度)`，`Seq` 由各自维护。

3. `TCP segment of a reassembled PDU`表示一个重新装配的数据协议单元的TCP报，也就是说数据协议单元被TCP传输层重新分割了。只有在传输最后一个TCP段的时候，会不显示该标记TCP segment of a reassembled PDU。

#### 2.2.1 客户端发送GET
![Alt text](/assets/ComputerNetwork/2024010903TCP/image-3.png)

通过上图不难发现，第一次客户端发送数据和第三次握手的`Seq` 和 `Ack` 相等。

![Alt text](/assets/ComputerNetwork/2024010903TCP/image-4.png)

应用层协议就是对TCP传输 `payload` 的一种定义

#### 2.2.2 服务器回复OK

![Alt text](/assets/ComputerNetwork/2024010903TCP/服务器回复.png)

### 2.3 TCP四次挥手

![Alt text](/assets/ComputerNetwork/2024010903TCP/HTTP四次挥手.png)

客户端发送FIN报文表示请求断开连接,仅仅表示客户端不在发送数据了,但是还能够接收数据

### 2.4 查看TCP传输数据

右击 -> Follow -> TCP Stream

![Alt text](/assets/ComputerNetwork/2024010903TCP/右击Follow.png)

![Alt text](/assets/ComputerNetwork/2024010903TCP/查看不同TCP流.png)

