#include <gst/check/gstcheck.h>
#include <gst/gstcaps.h>

int 
main (){

static GstStaticCaps caps_factory = GST_STATIC_CAPS ("video/x-msvideo""video/x-msvideo");

  GstCaps *caps = gst_static_caps_get (&caps_factory);

  g_print ("gst_caps_get_size(caps) = %d\n", gst_caps_get_size(caps));

  gst_caps_unref (caps);

  return 0;
}