#include"my-iusb.h"

/* 该宏展开会生成my_iusb_default_init函数 */
G_DEFINE_INTERFACE(MyIUsb, my_iusb, G_TYPE_INVALID);

static void
my_iusb_default_init(MyIUsbInterface *iface)
{

}

gchar *
my_iusb_read(MyIUsb *self)
{
  g_return_if_fail(MY_IS_IUSB(self));
  MY_IUSB_GET_INTERFACE (self)->read(self);

}

void
my_iusb_write(MyIUsb *self, const gchar *str)
{
  g_return_if_fail(MY_IS_IUSB(self));
  MY_IUSB_GET_INTERFACE (self)->write(self, str);
}