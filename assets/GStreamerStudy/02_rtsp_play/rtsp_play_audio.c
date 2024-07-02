#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it to callbacks */
/* 这里存下了所有需要的局部变量，因为本教程中会有回调函数，使用struct比较方便 */
typedef struct _CustomData
{
  GstElement *pipeline;
  GstElement *source;

  GstElement *h264depay;
  GstElement *h264parse;
  GstElement *h264decode;
  GstElement *video_convert;
  GstElement *video_sink;

  GstElement *rtpmp4gdepay;
  GstElement *aacparse;
  GstElement *avdec_aac;
  GstElement *audio_convert;
  GstElement *audio_sink;

} CustomData;

/* Handler for the pad-added signal */
static void pad_added_handler (GstElement * src, GstPad * pad,
    CustomData * data);

gboolean
print_pad_structure(GQuark   field_id,
                    const GValue * value,
                    gpointer user_data){

  g_print("****foreach****\n");
  g_print("%s\n", G_VALUE_TYPE_NAME(value));
  if( g_type_is_a(value->g_type, G_TYPE_INT))
    g_print("%d, %d\n",field_id,g_value_get_int(value));
  else
    g_print("%d, %s\n",field_id,g_value_get_string(value));

  return TRUE;
}

gboolean
gst_event_callback(GstPad *pad, GstObject *parent, GstEvent *event){
  g_print("***Event\n");
  return TRUE;
}

int
main (int argc, char *argv[])
{
  CustomData data;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  // g_setenv("GST_DEBUG_DUMP_DOT_DIR", "./", TRUE);

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  data.source = gst_element_factory_make ("rtspsrc", "source");

  /* 视频 */
  data.h264depay = gst_element_factory_make ("rtph264depay", "rtph264depay");
  data.h264parse = gst_element_factory_make ("h264parse", "h264parse");
  data.h264decode = gst_element_factory_make ("avdec_h264", "nvv4l2decoder"); // nvv4l2decoder avdec_h264
  data.video_convert = gst_element_factory_make ("videoconvert", "videoconvert");
  data.video_sink = gst_element_factory_make ("autovideosink", "video_sink"); //nv3dsink

  /* 音频 */
  data.rtpmp4gdepay = gst_element_factory_make ("rtpmp4gdepay", "rtpmp4gdepay");
  data.aacparse = gst_element_factory_make ("aacparse", "aacparse");
  data.avdec_aac = gst_element_factory_make ("avdec_aac", "avdec_aac");
  data.audio_convert = gst_element_factory_make ("audioconvert", "audioconvert");
  data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink"); 

  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");

  if (!data.pipeline || !data.source || !data.h264depay || !data.h264parse \
      || !data.h264decode || !data.video_convert || !data.video_sink \
      || !data.rtpmp4gdepay || !data.aacparse || !data.avdec_aac \
      || !data.audio_convert || !data.audio_sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (data.pipeline), data.source, 
      data.h264depay, data.h264parse, data.h264decode, data.video_convert, data.video_sink, \
      data.rtpmp4gdepay, data.aacparse, data.avdec_aac, data.audio_convert, data.audio_sink, NULL);


  if (!gst_element_link_many (data.h264depay, data.h264parse, \
                              data.h264decode, data.video_convert, data.video_sink, NULL)) {
    g_printerr ("Video Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.rtpmp4gdepay, data.aacparse, \
                              data.avdec_aac, data.audio_convert, data.audio_sink, NULL)) {
    g_printerr ("Audio Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  /* Set the URI to play */
  // g_object_set(data.source, "location", "rtsp://admin:EIOHDC@192.168.10.13:554/Streaming/Channels/101", \
  //                           "latency", 100, "protocols", 0x04, NULL);
  
  // g_object_set(data.source, "location", "rtsp://admin:yangquan321@192.168.2.17:554/Streaming/Channels/101", 
  //                           "latency", 0, NULL);

  g_object_set(data.source, "location", "rtsp://admin:yangquan123@192.168.11.221:554/Streaming/Channels/101", 
                            "latency", 0, NULL);


  // g_object_set(data.source, "location", "rtsp://admin:LHLQLW@192.168.2.18:554/Streaming/Channels/101", \
  //                           "latency", 100, "protocols", 0x04, NULL);

  /* Connect to the pad-added signal */
  /* 在这里把回调函数的src data变量指定参数*/
  g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler),
      &data);
  
  /* Start playing */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }
  
  /* Listen to the bus */
  bus = gst_element_get_bus (data.pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n",
              GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n",
              debug_info ? debug_info : "none");
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
            gst_message_parse_state_changed (msg, &old_state, &new_state,
                &pending_state);
            g_message ("Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state),
                gst_element_state_get_name (new_state));
            char state_name[100];
            g_snprintf (state_name, 100, "%s", gst_element_state_get_name (new_state));
            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(data.pipeline), GST_DEBUG_GRAPH_SHOW_ALL, state_name);
          }
          break;
        default:
          /* We should not reach here */
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);
  return 0;
}


static void
pad_added_handler (GstElement * src, GstPad * new_pad, CustomData * data)
{
  GstPad *sink_pad = NULL;
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;  
  
  new_pad_caps = gst_pad_get_current_caps (new_pad);
  gint new_pad_caps_size = gst_caps_get_size (new_pad_caps);

  for (int i = 0; i < new_pad_caps_size; i++) {
    new_pad_struct = gst_caps_get_structure (new_pad_caps, i);
    gchar *str = gst_structure_to_string (new_pad_struct);
    g_print ("%s\n\n", str);
    g_free (str);

    const gchar *name = gst_structure_get_name (new_pad_struct);

    if (!g_strcmp0 (name, "application/x-rtp")) {
      const gchar* media_type = gst_structure_get_string (new_pad_struct, "media");
      
      if (!g_strcmp0 (media_type, "audio")) {

        sink_pad = gst_element_get_static_pad (data->rtpmp4gdepay, "sink");
        
        g_print ("sink_pad = %p\n", sink_pad);

      } else if (!g_strcmp0 (media_type, "video")) {
        sink_pad = gst_element_get_static_pad (data->h264depay, "sink");
        
        g_print ("sink_pad = %p\n", sink_pad);
      }


    }

  }

  /* If our converter is already linked, we have nothing to do here */
  if (gst_pad_is_linked (sink_pad)) {
    g_message ("We are already linked. Ignoring.\n");
    goto exit;
  }

  /* Attempt the link */
  ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_message ("Type is '%s' but link failed. ret = %d\n", new_pad_type, ret);
  } else {
    g_message ("Link succeeded (type '%s').\n", new_pad_type);
  }

exit:
  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL)
    gst_caps_unref (new_pad_caps);

  if (sink_pad != NULL)
    gst_object_unref (sink_pad);
}
