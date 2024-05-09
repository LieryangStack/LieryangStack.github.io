/*************************************************************************************************************************************************
 * @filename: uart_reference.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Apr-22-2024
 * @brief: 1. Linux串口通信标准参考代码。
 * @note: 
 * @history: 
 *          1. Date:
 *             Author:
 *             Modification:
 *      
 *          2. ..
 *************************************************************************************************************************************************
*/

#define TIME_PER_MS 1000

#include <termios.h> /* 结构体 struct termios， 函数 tcgetattr()  tcsetattr() tcflush()  cfsetospeed() cfgetospeed() */
#include <fcntl.h> /* 函数 open()  */
#include <unistd.h> /* 函数 read() write() close() */
#include <glib.h>

static gint fd = 0; /* 串口设备文件描述符 */
static struct termios tc_old_cfg, tc_new_cfg; /* 终端配置 */

static gboolean
vpf_device_read_wind_speed (gpointer data){
  guint8 cmd[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38};
  write (fd, cmd, sizeof (cmd));
	g_usleep (TIME_PER_MS * 10);
  return TRUE;
}

static gboolean
vpf_device_send_alarm(gpointer data) {
  // guint8 cmd[] = {0x01, 0x06, 0x11, 0x03, 0x00, 0x01, 0xBD, 0x36};
	
	static gboolean light = TRUE;

	if (light) {
		guint8 cmd[] = {0xff, 0x06, 0x00, 0xc2, 0x00, 0x11, 0xfd, 0xe4};
		write(fd, cmd, sizeof(cmd));
		light = FALSE;
	} else {
		guint8 cmd[] = {0xff, 0x06, 0x00, 0xc2, 0x00, 0x60, 0x3d, 0xc0};
		write(fd, cmd, sizeof(cmd));
		light = TRUE;
	}
	g_usleep (TIME_PER_MS * 10);
  return TRUE;
}


static gboolean
vpf_device_read (GIOChannel *source, GIOCondition condition, gpointer data) {
	guint8 buffer[255];
	gint n = read(fd, &buffer, 255);
	for (gint i = 0; i < n; i++)
		g_print("0x%hhx ", buffer[i]);
	g_print("n = %d\n", n);
  
	return TRUE;
}

int 
main(int argc, char const *argv[]) {

	gint ret_val = 0; /* 该线程返回值 */
	GIOChannel *channel = NULL;

	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  
	/**
   * O_RDWR:    指示该设备文件可以进行读写操作。
   * O_NDELAY:  对文件描述符进行读和写不会阻塞线程，会立即返回。
   * O_NOCTTY:  是用来打开一个串口设备时，避免该串口设备成为进程的控制终端。（键盘可以是进程的控制终端，串口传输可能传输Ctr+C，意外关闭进程）
  */
  // fd = open("/dev/ttyUSB0", O_RDWR | O_NDELAY | O_NOCTTY);
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

	if (fd == -1) {
    g_print("Could not open serial port on /dev/ttyUSB0!\n");
		ret_val = fd;
    goto exit;
  }

	tcgetattr(fd, &tc_old_cfg); /* 保存之前的配置 */

	/* 配置串口 */
	cfsetispeed(&tc_new_cfg, B9600);  /* 指定输入波特率 */
	cfsetospeed(&tc_new_cfg, B9600);  /* 指定输出波特率 */

	/**
	 * c_lflag 本地模式 (我们是对以下标记取反，也就是说不启用以下相关功能)
	 * @param ICANON: 启用标准模式
	 * @param ECHO: 回显输入字符
	 * @param ECHOE: ECHOE如果同时设置了 ICANON，字符 ERASE 擦除前一个输入字符，WERASE 擦除前一个词
	 * @param ISIG: ISIG当接受到字符 INTR, QUIT, SUSP, 或 DSUSP 时，产生相应的信号
	*/
	tc_new_cfg.c_lflag &= ~(ICANON | ECHO | ECHOE |  ISIG);

	/**
	 * c_oflag 输出模式 (我们是对以下标记取反，也就是说不启用以下相关功能)
	 * @param OPOST: 处理后再输出
	 * @param ONLCR： 将输入的NL（换行）转换成CR（回车）
	 * @param OCRNL: 将输入的CR（回车）转换成NL（换行）
	*/
	tc_new_cfg.c_oflag &= ~(OPOST | ONLCR | OCRNL);

	/**
	 * c_iflag 输入模式 (我们是对以下标记取反，也就是说不启用以下相关功能)
	 * @param INPCK: 不启用输入奇偶检测
	 * @param ICRNL: 将输入的回车转化成换行（如果IGNCR未设置的情况下）
	 * @param INLCR: 将输入的NL（换行）转换成CR（回车）
	 * @param IXON: IXON启用输出的 XON/XOFF流控制
	 * @param IXOFF: IXOFF启用输入的 XON/XOFF流控制
	 * @param IXANY: (不属于 POSIX.1；XSI) 允许任何字符来重新开始输出
	*/
	tc_new_cfg.c_iflag &= ~(INPCK | ICRNL| INLCR | IXON | IXOFF | IXANY); 

	/* c_cflag 控制模式 */
	tc_new_cfg.c_cflag &= ~ CSIZE;   /* 清空传输字符长度标记位。 字符长度掩码,取值为 CS5, CS6, CS7, 或 CS8,加~就是无 */
	tc_new_cfg.c_cflag &= ~ CSTOPB;  /* 设置一个停止位。 CSTOPB设置两个停止位，而不是一个,加~就是设置一个停止位 */
	tc_new_cfg.c_cflag &= ~ PARENB;  /* 无奇偶校验。 PARENB允许输出产生奇偶信息以及输入的奇偶校验,加~就是无校验 */
	tc_new_cfg.c_cflag |=  CS8;      /* 传输8位数据 */
	tc_new_cfg.c_cflag |= CLOCAL;    /* CLOCAL表示忽略调制解调器控制线，串口通信没有该控制线 */
  tc_new_cfg.c_cflag |= CREAD;     /* 当 CREAD 标志被设置时，表示启用串行端口的接收器，允许设备接收数据 */

	/* c_cc[NCCS] 控制字符 */
	tc_new_cfg.c_cc[VTIME] = 1;
	tc_new_cfg.c_cc[VMIN] = 9;

	/*  清除串口输入输出缓存 */
	tcflush(fd, TCIOFLUSH);

	/* 设置终端控制属性,TCSANOW：不等数据传输完毕就立即改变属性 */
	tcsetattr(fd, TCSANOW, &tc_new_cfg);

	channel = g_io_channel_unix_new (fd);
	g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_ERR,(GIOFunc)vpf_device_read, NULL);

	// g_timeout_add_seconds (1, vpf_device_send_alarm, NULL);
	g_timeout_add (100, vpf_device_read_wind_speed, NULL);
	g_timeout_add (500, vpf_device_send_alarm, NULL);

	g_main_loop_run(loop);

exit:
	
	if (channel)
		g_io_channel_unref (channel);
	if (fd > 0)
    close(fd);
	if (ret_val == 0)
		 tcsetattr(fd, TCSANOW, &tc_old_cfg); /* 恢复到之前的配置 */

	return ret_val;
}
