#include "my-udisk.h"
 
static void my_iusb_interface_init (MyIUsbInterface *iface);
 
G_DEFINE_TYPE_WITH_CODE (MyUdisk, my_udisk, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MY_TYPE_IUSB, my_iusb_interface_init));
 
static gchar *
my_udisk_read (MyIUsb *iusb)
{
  MyUdisk *udisk = MY_UDISK (iusb);
  return udisk->data->str;
}

static void
my_udisk_write (MyIUsb *iusb, const gchar *str)
{
  MyUdisk *udisk = MY_UDISK (iusb);
  g_string_assign (udisk->data, str);
}

static void
my_udisk_init (MyUdisk *self)
{
  self->data = g_string_new (NULL);
}

static void
my_udisk_class_init (MyUdiskClass *self)
{
}

static void
my_iusb_interface_init (MyIUsbInterface *iface)
{
  iface->read = my_udisk_read;
  iface->write = my_udisk_write;
}