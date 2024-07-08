---
layout: post
title: 一、NVIDIA——显卡驱动、CUDA、cuDNN、TensorRt安装
categories: NVIDIA
tags: [NVIDIA]
---

NVIDIA显卡驱动和CUDA选择 `run` 文件进行安装，cuDNN和TensorRt选择 `tar` 文件解压放到指定目录，添加相关环境变量。

我这里暂时不适用apt进行安装，因为暂时没了解过apt安装软件包原理。

## 1 NVIDIA显卡驱动安装

### 1.1 禁用nouveau(nouveau是通用的驱动程序)（必须）

1. `lsmod | grep nouveau` 命令查看是否有输出

2. 如果有输出， `sudo gedit /etc/modprobe.d/blacklist.conf`末尾输入：

    ```
    blacklist nouveau

    options nouveau modeset=0
    ```
3.重启后，重新查看`lsmod | grep nouveau`是否有输出

### 1.2 安装依赖

```
sudo apt-get update   #更新软件列表
 
sudo apt-get install g++ gcc make pkg-config libglvnd-dev -y
```

### 1.3 卸载原有NVIDIA驱动

[搜索原有驱动下载链接：https://www.nvidia.com/Download/Find.aspx?lang=zh-cn](https://www.nvidia.com/Download/Find.aspx?lang=zh-cn)

**注意**：
要先进文本界面，停止显示管理器服务（参考1.4）。

```sh
sudo sh NVIDIA-Linux-x86_64-530.41.03.run --uninstall
# 如果使用apt库安装的驱动
sudo apt-get autoremove --purge nvidia*
```

### 1.4 安装驱动

下载相应驱动：[https://www.nvidia.cn/Download/index.aspx?lang=en-us](https://www.nvidia.cn/Download/index.aspx?lang=en-us)

```sh
sudo telinit 3  #进入文本界面
sudo service gdm3 stop   #停止显示服务
sudo chmod 777 NVIDIA-Linux-x86_64-535.146.02.run   #给你下载的驱动赋予可执行权限，才可以安装
sudo ./NVIDIA-Linux-x86_64-430.26.run    #安装
sudo  service  gdm3 start   #重启显示服务，完成
```

## 2 CUDA安装

CUDA是NVIDIA推出的用于自家GPU进行并行计算的框架，用户可通过CUDA的API调度GPU进行加速计算（只有当要解决的计算问题是可以大量并行计算时才能发挥CUDA的作用）

### 2.1 查看驱动驱动的CUDA版本

```sh
lieryang@lieryang-B760M-AORUS-ELITE-AX:~$ nvidia-smi
Sun Dec 17 21:27:42 2023       
+---------------------------------------------------------------------------------------+
| NVIDIA-SMI 535.146.02             Driver Version: 535.146.02   CUDA Version: 12.2     |
|-----------------------------------------+----------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |         Memory-Usage | GPU-Util  Compute M. |
|                                         |                      |               MIG M. |
|=========================================+======================+======================|
|   0  NVIDIA GeForce RTX 3060        Off | 00000000:01:00.0  On |                  N/A |
|  0%   30C    P8              23W / 170W |    813MiB / 12288MiB |      1%      Default |
|                                         |                      |                  N/A |
+-----------------------------------------+----------------------+----------------------+
                                                                                         
+---------------------------------------------------------------------------------------+
| Processes:                                                                            |
|  GPU   GI   CI        PID   Type   Process name                            GPU Memory |
|        ID   ID                                                             Usage      |
|=======================================================================================|
|    0   N/A  N/A     93803      G   /usr/lib/xorg/Xorg                          432MiB |
|    0   N/A  N/A     94032      G   /usr/bin/gnome-shell                         72MiB |
|    0   N/A  N/A     97301      G   ...sion,SpareRendererForSitePerProcess      116MiB |
|    0   N/A  N/A     98077    C+G   ...seed-version=20231214-180149.265000      167MiB |
|    0   N/A  N/A    104273      G   /usr/bin/gnome-control-center                 2MiB |
+---------------------------------------------------------------------------------------+

```

我安装的驱动版本是 `535.146.02`，对应 `CUDA Version: 12.2`。

### 2.2 runfile安装CUDA

[下载CUDA Toolkit链接：https://developer.nvidia.com/cuda-toolkit-archive](https://developer.nvidia.com/cuda-toolkit-archive)

我下载安装 `CUDA 12.2.1`。

![Alt text](/assets/NVIDIA/01_Install_ENV/CUDA下载.png)

```sh
# 安装方式
sudo sh cuda_12.2.1_535.86.10_linux.run
# 卸载方式
sudo /usr/local/cuda-12.2/bin/cuda-uninstaller
```

安装完成后会提示：

```sh
Driver:   Not Selected
Toolkit:  Installed in /usr/local/cuda-12.2/

Please make sure that
 -   PATH includes /usr/local/cuda-12.2/bin
 -   LD_LIBRARY_PATH includes /usr/local/cuda-12.2/lib64, or, add /usr/local/cuda-12.2/lib64 to /etc/ld.so.conf and run ldconfig as root
```

#### 2.2.1 添加CUDA环境变量

```sh
gedit ~/.bashrc # 添加CUD的PATH和LD_LIBRARY_PATH路径
source ~/.bashrc # 更新环境变量
```

![Alt text](/assets/NVIDIA/01_Install_ENV/CUDA环境变量添加.png)

#### 2.2.2 测试CUDA是否安装成功

```sh
lieryang@lieryang-B760M-AORUS-ELITE-AX:~$ nvcc -V
nvcc: NVIDIA (R) Cuda compiler driver
Copyright (c) 2005-2023 NVIDIA Corporation
Built on Tue_Jul_11_02:20:44_PDT_2023
Cuda compilation tools, release 12.2, V12.2.128
Build cuda_12.2.r12.2/compiler.33053471_0
```

#### 2.2.3 CUDA卸载

```sh
cd /usr/local/cuda-12.2/bin
sudo ./cuda-uninstaller
```

![alt text](/assets/NVIDIA/01_Install_ENV/image.png)

### 2.3 deb(network)安装CUDA

暂时没有研究


#### 2.5 多版本 CUDA 切换方式

Ubuntu中多版本CUDA切换：[https://blog.csdn.net/sinat_40245632/article/details/109330182](https://blog.csdn.net/sinat_40245632/article/details/109330182)



## 3 cuDNN安装

cuDNN是NVIDIA推出的用于自家GPU进行神经网络训练和推理的加速库，用户可通过cuDNN的API搭建神经网络并进行推理，cuDNN则会将神经网络的计算进行优化，再通过CUDA调用GPU进行运算，从而实现神经网络的加速（当然你也可以直接使用CUDA搭建神经网络模型，而不通过cuDNN，但运算效率会低很多）

### 3.1 cuDNN下载

[cuDNN下载地址：https://developer.nvidia.com/rdp/cudnn-archive](https://developer.nvidia.com/rdp/cudnn-archive)

我选择下载 `Local Installer for Linux x86_64 (Tar)`

![Alt text](/assets/NVIDIA/01_Install_ENV/cuDNN下载.png)

<font color="red">cuDNN最好使用deb方式安装，以免出现动态库软件链接报错</font>

### 3.2 deb方式安装cuDNN

```sh
sudo dpkg -i cudnn-local-repo-ubuntu2204-8.9.7.29_1.0-1_amd64.deb
sudo cp 根据提示拷贝一个key
sudo apt update
sudo apt install libcudnn8 # 这时候cudnn相关文件时安装到了 /usr/lib/x86_64-linux-gnu
```

![alt text](/assets/NVIDIA/01_Install_ENV/image-1.png)


### 3.3 tar方式安装cuDNN

```sh
# 解压cuDNN文件
tar -xvf cudnn-linux-x86_64-8.9.6.50_cuda12-archive.tar.xz 
# 进入cuDNN文件夹，cuda文件夹就是上面安装的cuda-12.2文件夹的软连接
cd cudnn-linux-x86_64-8.9.6.50_cuda12-archive
sudo cp include/cudnn*.h /usr/local/cuda/include
sudo cp -p lib/libcudnn* /usr/local/cuda/lib64
sudo chmod a+r /usr/local/cuda/include/cudnn*.h /usr/local/cuda/lib64/libcudnn*
```

### 3.4 tar测试cuDNN是否安装成功

```sh
lieryang@lieryang-B760M-AORUS-ELITE-AX:~/Downloads/cudnn-linux-x86_64-8.9.6.50_cuda12-archive$ cat /usr/local/cuda/include/cudnn_version.h | grep CUDNN_MAJOR -A 2
#define CUDNN_MAJOR 8
#define CUDNN_MINOR 9
#define CUDNN_PATCHLEVEL 6
--
#define CUDNN_VERSION (CUDNN_MAJOR * 1000 + CUDNN_MINOR * 100 + CUDNN_PATCHLEVEL)
```

## 4 TensorRt安装

TensorRT是可以在NVIDIA各种GPU硬件平台下运行的一个模型推理框架，支持C++和Python推理。即我们利用Pytorch，Tensorflow或者其它框架训练好的模型，可以转化为TensorRT的格式，然后利用TensorRT推理引擎去运行该模型，从而提升这个模型在NVIDIA-GPU上运行的速度。

TensorRt其实跟cuDNN有点类似，也是NVIDIA推出的针对自家GPU进行模型推理的加速库，只不过它不支持训练，只支持模型推理。相比于cuDNN，TensorRt在执行模型推理时可以做到更快。

### 4.1 TensorRt下载

[下载链接：https://developer.nvidia.com/tensorrt-download](https://developer.nvidia.com/tensorrt-download)

这里选择 `TensorRT 8.6 GA for Linux x86_64 and CUDA 12.0 and 12.1 TAR Package` 进行下载安装

![Alt text](/assets/NVIDIA/01_Install_ENV/TensorRt下载.png)

### 4.2 TensorRt安装

```sh
# 解压
tar -zxvf TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-12.0.tar.gz
# 把文件夹内容都拷贝到 /usr/local
cd TensorRT-8.6.1.6.Linux.x86_64-gnu.cuda-12.0
sudo cp -r TensorRT-8.6.1.6 /usr/local/
# 添加TensorRt环境变量
sudo gedit ~/.bashrc #复制到最后面
# 把以下两行内容添加到 .bashrc 文件最后面
export PATH=$PATH:/usr/local/TensorRT-8.6.1.6/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/TensorRT-8.6.1.6/lib
# 保存后刷新环境变量
source ~/.bashrc

# 设定软连接
cd /usr/local
sudo ln -s TensorRT-8.6.1.6 TensorRT
```

### 4.3 测试TensorRt是否安装成功

```sh
# 进入到 TensorRT-8.6.1.6/samples/sampleOnnxMNIST
cd TensorRT-8.6.1.6/samples/sampleOnnxMNIST
# 设定环境变量
export CUDA_INSTALL_DIR=/usr/local/cuda
export CUDNN_INSTALL_DIR=/usr/local/cuda
export TRT_LIB_DIR=/usr/local/TensorRT/lib
export PROTOBUF_INSTALL_DIR=/usr/lib/x86_64-linux-gnu
# 编译
sudo make -j 10
# 进入到bin文件夹测试运行
cd TensorRT-8.6.1.6/bin
```

![Alt text](/assets/NVIDIA/01_Install_ENV/TensorRt运行.png)


