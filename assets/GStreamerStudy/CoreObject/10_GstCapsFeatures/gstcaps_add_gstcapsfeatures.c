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

  /**
   * note: 因为这是从 GstStaticCaps 中获取的 caps
   *       所以该 caps 等于 caps_factory.caps，所以caps不能修改，防止修改原始的GstStaticCaps.caps
   */
  GstCaps *caps1 = gst_static_caps_get (&caps_factory);
  g_print ("caps1 = %p\n", caps1);
  g_print ("caps_factory.caps = %p\n", caps_factory.caps);


  GstCaps *caps2 = gst_caps_new_simple ("video", "int", G_TYPE_INT, 4, "float", G_TYPE_FLOAT, 5.7, NULL);

  /* 从 GstStaticCaps 获取的 GstCaps 不可写 */
  g_print ("gst_caps_is_writable (caps1) = %d\n", gst_caps_is_writable (caps1));
  /* 其余方式创建的 GstCaps 可写 */
  g_print ("gst_caps_is_writable (caps2) = %d\n", gst_caps_is_writable (caps2));

  GstCapsFeatures *features = gst_caps_features_new ("memory:NVMM", NULL);

  gst_caps_set_features (caps2, 0, features);

  GstCaps *caps1_write = (GstCaps *)gst_mini_object_make_writable (GST_MINI_OBJECT(caps1));
  for (int i = 0; i < gst_caps_get_size(caps1_write); i++) {
    features = gst_caps_features_new ("memory:NVMM", NULL);
    gst_caps_set_features (caps1_write, i, features);
  }
  gst_caps_unref (caps1);
  gst_caps_unref (caps1_write);
  gst_caps_unref (caps2);


  GstStructure *structure = gst_structure_from_string ("video/x-raw, width=(int)[1, 0x7fffffff]", NULL);

  return 0;
}


