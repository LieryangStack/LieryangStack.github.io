#define _GNU_SOURCE //在源文件开头定义_GNU_SOURCE 宏
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>

#include <time.h>


static struct termios oldt, newt;
static int fd;
/* UI界面读卡卡号记录 */
gchar *hex_str = NULL;
volatile gboolean cb_finish_flag = FALSE;

static GThread *vpf_device_read_thread = NULL;

GThread *vpf_app_device_thread = NULL;
GMainLoop *vpf_app_device_loop = NULL;
GMainContext *vpf_app_device_context = NULL;
GMutex vpf_device_data_mutex;

static gboolean 
id_card_data_check (guint8 *str, guint len) {
  guint i;
  guint8 checksum = 0;
  for (i = 0; i < len - 1; i++) {
    checksum ^= str[i];
  }
  checksum = ~checksum;
  if ( str[len-1] == checksum )
    return TRUE;
  else
    return FALSE;
}

static gboolean
wind_speed_data_check (guint8 *data, guint len) {
  guint16 crc = 0xFFFF;
  for (gint i = 0; i < len - 2; i++) {
    crc ^= (guint16)data[i];
    for (guint8 j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  // g_print ("crc[L] = %X, crc[H] = %X\n", (guint8)crc, (guint8)(crc >> 8));
  if ((guint8)(crc >> 8) == data[len-1] && (guint8)crc == data[len-2])
    return TRUE;
  else 
    return FALSE;
}

static gboolean
vpf_device_read_wind_speed (gpointer data){
  guint8 cmd[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
  write (fd, cmd, sizeof (cmd));
  return TRUE;
}

gboolean
vpf_device_send_alarm (gpointer data) {
  guint8 cmd[] = {0x01, 0x06, 0x11, 0x03, 0x00, 0x01, 0xBD, 0x36};
  write (fd, cmd, sizeof (cmd));
  return FALSE;
}

/**
 * UI线程直接调用，可以直接刷新界面
*/
gboolean
vpf_device_read_card (gpointer data) {
  /**
   * cmd[0]: 0x01 卡片相关操作命令（读卡号，读数据块，写数据块等操作）
   * cmd[1]: 0x08 整个数据包的长度
   * cmd[2]: 0xA1 读卡号命令，无需验证密钥
   * cmd[3]: 0x20 读卡器地址
   * cmd[4]:      保留
   * cmd[5]: 0x01 状态提示开
   * cmd[6]:      保留
   * cmd[7]:      校验和
  */
  guint8 cmd[] = {0x01, 0x08, 0xA1, 0x20, 0x00, 0x01, 0x00, 0x76};
  write (fd, cmd, sizeof (cmd));
  return FALSE;
}

static void
vpf_print_data (char *data, gint len) {
  for (int i = 0; i < len; i++)
    printf("%2X ", data[i]);
  printf("  n = %d \n", len);
}


/**
** 信号处理函数，当串口有数据可读时，会跳转到该函数执行
**/
static void 
io_handler(int sig, siginfo_t *info, void *context) {

  unsigned char buf[10] = {0};
  int ret;
  int n;

  time_t current_time;       // 存储系统时间
  struct tm * time_info;     // 存储本地时间
  char time_string[100];     // 存储格式化的时间字符串

  if(SIGRTMIN != sig)
      return;

  time(&current_time);                     // 获取当前系统时间
  time_info = localtime(&current_time);    // 转换为本地时间

  strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

  /* 判断串口是否有数据可读 */
  if (POLL_IN == info->si_code) {
    ret = read(fd, buf, 9); //一次最多读 8 个字节数据
    for (n = 0; n < ret; n++)
        printf("0x%hhx ", buf[n]);
    printf("n = %d %s\n", ret, time_string);
  } 
}


/**
** 异步 I/O 初始化函数
**/
static void 
async_io_init(void) {

  struct sigaction sigatn;
  int flag;

  clock_t start;

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


int
main (int argc, char *argv[]) {

  vpf_app_device_context = g_main_context_new ();
  vpf_app_device_loop = g_main_loop_new (vpf_app_device_context, FALSE);
  g_main_context_push_thread_default (vpf_app_device_context);

  /* 获取标准输入的终端参数，保存到oldt */
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  /* 禁用了规范模式和回显功能 */
  newt.c_lflag &= ~(ICANON | ECHO);
  /* 将修改的终端参数写入，这里使用 TCSANOW 表示立即生效
   * "Terminal Control Set Attribute"
   */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  /**
   * O_RDWR:    指示该设备文件可以进行读写操作。
   * O_NDELAY:  表示在打开设备文件时不会阻塞进程，即使该设备尚未准备好。
   *            如果该标志未设置，程序将一直等待设备就绪，可能会阻塞进程。
   * O_NOCTTY:  是用来打开一个串口设备时，避免该串口设备成为进程的控制终端的一个选项
  */
  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY );

  if (fd == -1) {
    printf("Could not open serial port on /dev/ttyUSB0!\n");
    goto exit;
  } else {
    /* 这行代码的作用是将之前打开的串行端口文件描述符(fd)的阻塞模式关闭，
     * 使得后续的读写操作不会阻塞进程。 
    */
    fcntl(fd, F_SETFL, 0);
  }

  // UART settings
  struct termios termAttr;
  tcgetattr(fd,&termAttr);
  cfsetispeed(&termAttr,B9600);                       // Input speed
  cfsetospeed(&termAttr,B9600);                       // Output speed
  termAttr.c_iflag = 0;                               // Turn off input processing
  termAttr.c_oflag = 0;                               // Turn of output processing
  termAttr.c_lflag = 0;                               // Turn off line procesinng
  termAttr.c_cflag = 0;                               // Turn off character processing
  termAttr.c_cflag |= (CS8|CREAD|CLOCAL);             // Read 8 bit
  termAttr.c_cc[VMIN] = 9;                            // No minimal chars
  termAttr.c_cc[VTIME] = 0;                           // Wait 0.1 s for input
  tcsetattr(fd,TCSANOW,&termAttr);   // Save settings

  /* Flush anything already in the serial buffer */
  tcflush(fd, TCIFLUSH);
  tcflush(STDIN_FILENO, TCIFLUSH);

  // vpf_device_read_thread = g_thread_new ("vpf.device.read.thread", \
  //                                vpf_device_read_thread_func, NULL);

  async_io_init();
  

  /* 读取风速 */
  GSource *source = g_timeout_source_new_seconds(1);
  //GSource *source = g_timeout_source_new(200);
  g_source_set_callback (source, vpf_device_read_wind_speed, NULL, NULL);
  guint vpf_read_wind_timeout_id = g_source_attach (source, vpf_app_device_context);
  g_source_unref (source);

  g_timeout_add_seconds (1, vpf_device_read_wind_speed, NULL);

  g_main_loop_run (vpf_app_device_loop);

  g_source_remove (vpf_read_wind_timeout_id);

  g_mutex_clear (&vpf_device_data_mutex);

exit:
  /* 等待一个线程完成，并且会把vpf_device_read_thread资源释放 */
  if (vpf_device_read_thread)
    g_thread_join (vpf_device_read_thread);
  /*free of resource*/
  // close serial
  if (fd)
    close(fd);
  // tty reset settings
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

  return 0;
}
