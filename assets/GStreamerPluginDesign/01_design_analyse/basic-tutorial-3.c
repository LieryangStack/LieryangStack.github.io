#include <gst/gst.h>

#define PRINT 0

typedef struct _CustomData CustomData;
struct _CustomData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *demux;

  GstElement *audio_queue;
  GstElement *vorbisdec;
  GstElement *audio_convert; /* 格式之间转换 */
  GstElement *audio_resample;  /* 音频采样率的转换 */
  GstElement *audio_sink;

//  GstElement *video_queue;
  GstElement *video_vp8dec;
  GstElement *video_convert;
  GstElement *video_sink;
};

static void
pad_added_handler (GstElement *src, GstPad *pad, CustomData *data);

int 
main (int argc, char *argv[]) {
  
  CustomData data;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  gst_init (&argc, &argv);


  data.source = gst_element_factory_make ("souphttpsrc", "source");
  data.demux = gst_element_factory_make ("matroskademux", "demux");

  data.video_vp8dec = gst_element_factory_make ("vp8dec", "video_vp8dec");
  data.video_convert = gst_element_factory_make ("videoconvert", "video_convert");
  data.video_sink = gst_element_factory_make ("autovideosink", "video_sink");

  data.pipeline = gst_pipeline_new ("test-pipeline");

#if PRINT
  g_print ("source_refcount = %d after gst_element_factory_make()...\n", GST_OBJECT_REFCOUNT_VALUE (data.source));
  g_print ("pipeline_refcount = %d after gst_element_factory_make()...\n", GST_OBJECT_REFCOUNT_VALUE (data.pipeline));
#endif

  if (!data.pipeline || !data.source || !data.demux || \
      !data.video_vp8dec || ! data.video_convert || !data.video_sink) {
    g_printerr ("Not all elements could be created!.\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.demux, \
                      data.video_vp8dec, data.video_convert, data.video_sink, NULL);

  if (!gst_element_link_many (data.source, data.demux, NULL)) {
    g_printerr ("src elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.video_vp8dec, \
                                data.video_convert, data.video_sink, NULL)) {
    g_printerr ("Video elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  g_object_set (data.source, "location", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

  g_signal_connect (data.demux, "pad-added", G_CALLBACK (pad_added_handler), &data);

  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  bus = gst_element_get_bus (data.pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, \
                        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          terminate = TRUE;
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          terminate = TRUE;
          break;
        case GST_MESSAGE_STATE_CHANGED:
          /* We are only interested in state-changed messages from the pipeline */
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            g_print ("Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
          }
          break;
        default:
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  g_print ("application exiting...\n");

  gst_object_unref (bus);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);

#if PRINT
  g_print ("source_refcount = %d before gst_object_unref (data.pipeline)\n", GST_OBJECT_REFCOUNT_VALUE (data.source));
  g_print ("pipeline_refcount = %d before gst_object_unref (data.pipeline)\n", GST_OBJECT_REFCOUNT_VALUE (data.pipeline));
#endif

  /* 这块内存已经被释放，但是指向该地址的指针没有赋值为NULL */
  gst_object_unref (data.pipeline);

  /* 调用gst_object_unref减少一个引用数，并且给 ata.pipeline = NULL */
  //gst_clear_object (&(data.pipeline));

#if PRINT
  g_print ("source_refcount = %d after gst_object_unref (data.pipeline)\n", GST_OBJECT_REFCOUNT_VALUE (data.source));
  g_print ("pipeline_refcount = %d after gst_object_unref (data.pipeline)\n", GST_OBJECT_REFCOUNT_VALUE (data.pipeline));
#endif

  return 0;
}

static void 
pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data){
  GstPadLinkReturn ret;
  GstPad *sink_pad = NULL;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;


  g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

  new_pad_caps = gst_pad_get_current_caps (new_pad);
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);

  if (g_str_has_prefix (new_pad_type, "video")) {
    sink_pad = gst_element_get_static_pad (data->video_vp8dec, "sink");
    /* Attempt the link */
    ret = gst_pad_link (new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
      g_print ("Type is '%s' but link failed.\n", new_pad_type);
    } else {
      g_print ("Link succeeded (type '%s').\n", new_pad_type);
    }
    
  } else if (0) {
    sink_pad = gst_element_get_static_pad (data->audio_queue, "sink");
    /* Attempt the link */
    ret = gst_pad_link (new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
      g_print ("Type is '%s' but link failed.\n", new_pad_type);
    } else {
      g_print ("Link succeeded (type '%s').\n", new_pad_type);
    }
    
  } else {
    goto exit;
  }

exit :
  if (new_pad_caps != NULL) {
    gst_caps_unref (new_pad_caps);
  }
  if (sink_pad != NULL) {
    gst_object_unref (sink_pad);
  }
}
