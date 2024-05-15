#include <glib.h>
#include <sys/eventfd.h>
#include <unistd.h>

int
main (int argc, char *argv[]) {

  gint efd = eventfd (1, 0);
  guint64 u = 12;

  g_print ("efd = %d\n", efd);

  // while (1);

  /* 由于初始化 @count = 1，所以此时计数器值 == 1 */
  write (efd, &u, 8);

  /* 上面一行写入一个无符号64位整形， 计数器累加 = 1 + 12 = 13*/
  read (efd, &u, 8);
  g_print ("u = %ld\n", u);

  /* 读之后计数器会清零 */
  u = 15;
  write (efd, &u, 8);

  /* 计数器值 = 0 + 15 = 15 */
  read (efd, &u, 8);
  g_print ("u = %ld\n", u);

  /* 此时计数器的值为零，不可读，所以会堵塞 */
  // read (efd, &u, 8);

  return 0;
}