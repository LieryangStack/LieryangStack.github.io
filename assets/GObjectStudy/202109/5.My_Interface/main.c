#include "my-udisk.h"

int
main (void)
{
  g_type_init ();

  MyUdisk *udisk = g_object_new (MY_TYPE_UDISK, NULL);

  my_iusb_write (MY_IUSB (udisk), "I am u-disk!");
  gchar *data = my_iusb_read (MY_IUSB (udisk));

  g_printf ("%s\n\n", data);

  g_printf ("Is udisk a MyIUsb object?\n");
  if (MY_IS_IUSB (udisk))
          g_printf ("Yes!\n");
  else
          g_printf ("No!\n");

  g_printf ("\nIs udisk a MyUdisk object?\n");
  if (MY_IS_UDISK (udisk))
          g_printf ("Yes!\n");
  else
          g_printf ("No!\n");

  return 0;
}