#include <gst/check/gstcheck.h>
#include <gst/gstcaps.h>

static GstStaticCaps caps_factory = GST_STATIC_CAPS ( 
  "audio/x-adpcm, layout = (string) swf, channels = (int) { 1, 2 }, rate = (int) { 5512, 11025, 22050, 44100 }; "
  "audio/mpeg, mpegversion = (int) 1, layer = (int) 3, channels = (int) { 1, 2 }, rate = (int) { 5512, 8000, 11025, 22050, 44100 }, parsed = (boolean) TRUE; "
  "audio/mpeg, mpegversion = (int) { 4, 2 }, stream-format = (string) raw; "
  "audio/x-nellymoser, channels = (int) { 1, 2 }, rate = (int) { 5512, 8000, 11025, 16000, 22050, 44100 }; "
  "audio/x-raw, format = (string) { U8, S16LE}, layout = (string) interleaved, channels = (int) { 1, 2 }, rate = (int) { 5512, 11025, 22050, 44100 }; "
  "audio/x-alaw, channels = (int) { 1, 2 }, rate = (int) 8000; "
  "audio/x-mulaw, channels = (int) { 1, 2 }, rate = (int) 8000; "
  "audio/x-speex, channels = (int) 1, rate = (int) 16000;");

int 
main (){

  gst_init (NULL, NULL);

  GstCaps *caps1 = gst_static_caps_get (&caps_factory);

  GstCaps *caps2 = gst_static_caps_get (&caps_factory);

  GstCaps *caps1_write = (GstCaps *)gst_mini_object_make_writable (GST_MINI_OBJECT(caps1));

  gst_caps_append (caps1_write, caps2);

  g_print ("gst_caps_get_size(caps) = %d\n", gst_caps_get_size(caps1_write));

  gst_caps_unref (caps1);
  gst_caps_unref (caps1_write);

  return 0;
}