/***************************************************************
Copyright © OneFu Co., Ltd. 1998-2022. All rights reserved.
文件名 : uart.c
作者 : waterfxw
版本 : V1.0
描述 : uart 示例代码
其他 : 主要是测试 使用cmake
日志 : 初版 V1.0 2023/03/20 waterfxw创建
***************************************************************/

#define _GNU_SOURCE //在源文件开头定义_GNU_SOURCE 宏
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>



typedef struct uart_hardware_cfg {
    unsigned int baudrate;          /* 波特率 */
    unsigned char dbit;             /* 数据位 */
    char parity;                    /* 奇偶校验 */
    unsigned char sbit;             /* 停止位 */
} uart_cfg_t;

static struct termios old_cfg;      //用于保存终端的配置参数
static int fd;                      //串口终端对应的文件描述符


/**
** 串口初始化操作
** 参数 device 表示串口终端的设备节点
**/
static int uart_init(const char *device) {
    /* 打开串口终端 */
    fd = open(device, O_RDWR | O_NOCTTY);
    if (0 > fd) {
        fprintf(stderr, "open error: %s: %s\n", device, strerror(errno));
        return -1;
    }

    /* 获取串口当前的配置参数 */
    if (0 > tcgetattr(fd, &old_cfg)) {
        fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    return 0; 
}


/**
** 串口配置
** 参数 cfg 指向一个 uart_cfg_t 结构体对象
**/
static int uart_cfg(const uart_cfg_t *cfg) {
    struct termios new_cfg = {0}; //将 new_cfg 对象清零
    speed_t speed;
    /* 设置为原始模式 */
    cfmakeraw(&new_cfg);
    /* 使能接收 */
    new_cfg.c_cflag |= CREAD;
    /* 设置波特率 */
    switch (cfg->baudrate) {
        case 1200: speed = B1200;
            break;
        case 1800: speed = B1800;
            break;
        case 2400: speed = B2400;
            break;
        case 4800: speed = B4800;
            break;
        case 9600: speed = B9600;
            break;
        case 19200: speed = B19200;
            break;
        case 38400: speed = B38400;
            break;
        case 57600: speed = B57600;
            break;
        case 115200: speed = B115200;
            break;
        case 230400: speed = B230400;
            break;
        case 460800: speed = B460800;
            break;
        case 500000: speed = B500000;
            break;
        default: //默认配置为 115200
            speed = B115200;
            printf("default baud rate: 115200\n");
            break;
    }

    if (0 > cfsetspeed(&new_cfg, speed)) {
        fprintf(stderr, "cfsetspeed error: %s\n", strerror(errno));
        return -1;
    }
    /* 设置数据位大小 */
    new_cfg.c_cflag &= ~CSIZE; //将数据位相关的比特位清零
    switch (cfg->dbit) {
        case 5:
            new_cfg.c_cflag |= CS5;
            break;
        case 6:
            new_cfg.c_cflag |= CS6;
            break;
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
        default: //默认数据位大小为 8
            new_cfg.c_cflag |= CS8;
            printf("default data bit size: 8\n");
            break;
    }

    /* 设置奇偶校验 */
    switch (cfg->parity) {
        case 'N': //无校验
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        break;
    case 'O': //奇校验
        new_cfg.c_cflag |= (PARODD | PARENB);
        new_cfg.c_iflag |= INPCK;
        break;
    case 'E': //偶校验
        new_cfg.c_cflag |= PARENB;
        new_cfg.c_cflag &= ~PARODD; /* 清除 PARODD 标志，配置为偶校验 */
        new_cfg.c_iflag |= INPCK;
        break;
    default: //默认配置为无校验
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        printf("default parity: N\n");
        break;
    }


    /* 设置停止位 */
    switch (cfg->sbit) {
        case 1: //1 个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2: //2 个停止位
            new_cfg.c_cflag |= CSTOPB;
            break;
        default: //默认配置为 1 个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            printf("default stop bit size: 1\n");
            break;
    }


    /* 将 MIN 和 TIME 设置为 0 */
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    /* 清空缓冲区 */
    if (0 > tcflush(fd, TCIOFLUSH)) {
        fprintf(stderr, "tcflush error: %s\n", strerror(errno));
        return -1;
    }

    /* 写入配置、使配置生效 */
    if (0 > tcsetattr(fd, TCSANOW, &new_cfg)) {
        fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
        return -1;
    }

    /* 配置 OK 退出 */
    return 0; 
 }


/***
--dev=/dev/ttyUSB0
--brate=115200
--dbit=8
--parity=N
--sbit=1
--type=read
***/
/**打印帮助信息**/
static void show_help(const char *app) {
 printf("Usage: %s [选项]\n"
 "\n 必选选项:\n"
 " --dev=DEVICE 指定串口终端设备名称, 譬如--dev=/dev/ttyUSB0\n"
 " --type=TYPE 指定操作类型, 读串口还是写串口, 譬如--type=read(read 表示读、write 表示写、readwrite表示读写、其它值无效)\n"
 "\n 可选选项:\n"
 " --brate=SPEED 指定串口波特率, 譬如--brate=115200\n"
 " --dbit=SIZE 指定串口数据位个数, 譬如--dbit=8(可取值为: 5/6/7/8)\n"
 " --parity=PARITY 指定串口奇偶校验方式, 譬如--parity=N(N 表示无校验、O 表示奇校验、E 表示偶校验)\n"
 " --sbit=SIZE 指定串口停止位个数, 譬如--sbit=1(可取值为: 1/2)\n"
 " --help 查看本程序使用帮助信息\n\n", app);
}


/**
** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
**/
static void io_handler(int sig, siginfo_t *info, void *context) {
    unsigned char buf[10] = {0};
    int ret;
    int n;
    if(SIGRTMIN != sig)
        return;

    /* 判断串口是否有数据可读 */
    if (POLL_IN == info->si_code) {
        ret = read(fd, buf, 8); //一次最多读 8 个字节数据
        printf("[ ");
        for (n = 0; n < ret; n++)
            printf("0x%hhx ", buf[n]);
        printf("]\n");
    } 
}


/**
** 异步 I/O 初始化函数
**/
static void async_io_init(void) {
    struct sigaction sigatn;
    int flag;

    /* 使能异步 I/O */
    flag = fcntl(fd, F_GETFL); //使能串口的异步 I/O 功能
    flag |= O_ASYNC;
    fcntl(fd, F_SETFL, flag);

    /* 设置异步 I/O 的所有者 */
    fcntl(fd, F_SETOWN, getpid());

    /* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
    fcntl(fd, F_SETSIG, SIGRTMIN);

    /* 为实时信号 SIGRTMIN 注册信号处理函数 */
    sigatn.sa_sigaction = io_handler; //当串口有数据可读时，会跳转到 io_handler 函数
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN, &sigatn, NULL);
}



int main
(int argc, char *argv[])
{
    uart_cfg_t cfg = {0};
    char *device = "/dev/ttyUSB0";
    int rw_flag = -1;
    unsigned char w_buf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //通过串口发送出去的数据
    int n;

    cfg.baudrate = 9600; /* 波特率 */
    cfg.dbit = 8; /* 串口位数 */
    cfg.sbit = 1; /* 串口停止位 */
    cfg.parity = 'N'; /* 表示无校验 */
    rw_flag = 2; //读写

    if (NULL == device || -1 == rw_flag) {
        fprintf(stderr, "Error: the device and read|write type must be set!\n");
        show_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("串口初始化 \n");
    /* 串口初始化 */
    if (uart_init(device))
        exit(EXIT_FAILURE);

    printf("串口配置 \n");
    /* 串口配置 */
    if (uart_cfg(&cfg)) {
        tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("读|写串口  \n");
    /* 读|写串口 */
    switch (rw_flag) {
        case 0: //读串口数据
            async_io_init(); //我们使用异步 I/O 方式读取串口的数据，调用该函数去初始化串口的异步 I/O
            for ( ; ; )
            sleep(1); //进入休眠、等待有数据可读，有数据可读之后就会跳转到 io_handler()函数
            break;
        case 1: //向串口写入数据
            for ( ; ; ) { //循环向串口写入数据
                write(fd, w_buf, 8); //一次向串口写入 8 个字节
                sleep(1); //间隔 1 秒钟
            }
            break;
        case 2: //向串口写入数据
            async_io_init(); //我们使用异步 I/O 方式读取串口的数据，调用该函数去初始化串口的异步 I/O
            for ( ; ; ) { //循环向串口写入数据
                write(fd, w_buf, 8); //一次向串口写入 8 个字节
                sleep(2); //间隔 2 秒钟
            }
            break;
    }

    /* 退出 */
    tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
    close(fd);
    exit(EXIT_SUCCESS);
}

