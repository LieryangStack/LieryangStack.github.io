#ifndef MY_IUSB_H
#define MY_IUSB_H
  
#include <glib-object.h>
  
#define MY_TYPE_IUSB (my_iusb_get_type ())
#define MY_IUSB(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),MY_TYPE_IUSB, MyIUsb))
#define MY_IS_IUSB(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_IUSB))
#define MY_IUSB_GET_INTERFACE(obj) (\
                G_TYPE_INSTANCE_GET_INTERFACE ((obj), MY_TYPE_IUSB, MyIUsbInterface))
  
typedef struct _MyIUsb MyIUsb;
typedef struct _MyIUsbInterface MyIUsbInterface;
  
struct _MyIUsbInterface {
    GTypeInterface parent_interface;
  
    gchar * (*read) (MyIUsb *self);
    void (*write) (MyIUsb *self, const gchar *str);
};
 
 
GType my_iusb_get_type (void);
 
gchar * my_iusb_read (MyIUsb *self);
void my_iusb_write (MyIUsb *self, const gchar *str);
  
#endif