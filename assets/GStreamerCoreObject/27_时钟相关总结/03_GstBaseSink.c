#include <gst/gst.h>

/* 这里存下了所有需要的局部变量，因为本教程中会有回调函数，使用struct比较方便 */
typedef struct _CustomData
{
  GMainLoop *loop;

  GstElement *pipeline;
  GstElement *source;

  GstElement *h265depay;
  GstElement *h265parse;
  GstElement *h265parse_tee;
  GstElement *h265decode;
  GstElement *video_convert;
  GstElement *video_sink;

  GstElement *rtpmp4gdepay;
  GstElement *aacparse;
  GstElement *aacparse_tee;
  GstElement *avdec_aac;
  GstElement *audio_convert;
  GstElement *audio_sink;

  GstElement *h265_parse_filter;
  GstElement *record_h265_parse;
  GstElement *qtmux;
  GstElement *filesink;

} CustomData;


static void
pad_added_handler (GstElement * src, GstPad * new_pad, CustomData * data)
{
  GstPad *sink_pad = NULL;
  
  GstCaps *new_pad_caps = gst_pad_get_current_caps (new_pad);
  gint new_pad_caps_size = gst_caps_get_size (new_pad_caps);

  for (int i = 0; i < new_pad_caps_size; i++) {
    
    /* 从数组中直接获取地址，所以我们不需要释放 new_pad_struct */
    GstStructure *new_pad_struct = gst_caps_get_structure (new_pad_caps, i);
    gchar *str = gst_structure_to_string (new_pad_struct);
    g_print ("%s\n\n", str);
    g_free (str);

    const gchar *name = gst_structure_get_name (new_pad_struct);

    if (!g_strcmp0 (name, "application/x-rtp")) {
      const gchar* media_type = gst_structure_get_string (new_pad_struct, "media");
      
      if (!g_strcmp0 (media_type, "audio")) {
        sink_pad = gst_element_get_static_pad (data->rtpmp4gdepay, "sink");
      } else if (!g_strcmp0 (media_type, "video")) {
        sink_pad = gst_element_get_static_pad (data->h265depay, "sink");
      }
    }
  }

  if (gst_pad_is_linked (sink_pad)) {
    g_message ("We are already linked. Ignoring.\n");
    goto exit;
  }

  /* Attempt the link */
  GstPadLinkReturn ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_message ("link failed. \n");
  }

exit:

  if (new_pad_caps != NULL)
    gst_caps_unref (new_pad_caps);

  if (sink_pad != NULL)
    gst_object_unref (sink_pad);
}


static void
print_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
  int i, num;

  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    const GValue *val;

    /* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    val = gst_tag_list_get_value_index (list, tag, i);
    if (G_VALUE_HOLDS_STRING (val)) {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      g_print ("\t%20s : %s\n", tag,
          (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      GstBuffer *buf = gst_value_get_buffer (val);
      guint buffer_size = gst_buffer_get_size (buf);

      g_print ("\t%20s : buffer of size %u\n", tag, buffer_size);
    } else if (GST_VALUE_HOLDS_DATE_TIME (val)) {
      GstDateTime *dt = g_value_get_boxed (val);
      gchar *dt_str = gst_date_time_to_iso8601_string (dt);

      g_print ("\t%20s : %s\n", tag, dt_str);
      g_free (dt_str);
    } else {
      g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }
}



static gboolean
my_bus_callback (GstBus * bus, GstMessage * message, gpointer user_data)
{
  CustomData *data = (CustomData *)user_data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("%s Error: %s\n",GST_OBJECT_NAME (message->src), err->message);
      g_error_free (err);
      g_free (debug);
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(data->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "error");
      if (GST_MESSAGE_SRC (message) == GST_OBJECT (data->video_sink)) {
        GstEvent *event = gst_event_new_eos ();
        gst_element_send_event (data->pipeline, event);
      }

      // g_main_loop_quit (data->loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print ("receive EOS\n");
      g_main_loop_quit (data->loop);
      break;
    case GST_MESSAGE_STATE_CHANGED:
      /* We are only interested in state-changed messages from the pipeline */
      if (GST_MESSAGE_SRC (message) == GST_OBJECT (data->pipeline)) {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed (message, &old_state, &new_state,
            &pending_state);
        g_message ("Pipeline state changed from %s to %s:\n",
            gst_element_state_get_name (old_state),
            gst_element_state_get_name (new_state));
        char state_name[100];
        g_snprintf (state_name, 100, "%s", gst_element_state_get_name (new_state));
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(data->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, state_name);
      }
      break;
    case GST_MESSAGE_TAG:
      // GstTagList *tags = NULL;
      // gst_message_parse_tag (message, &tags);
      // g_print ("Got tags from element %s:\n", GST_OBJECT_NAME (message->src));
      // gst_tag_list_foreach (tags, print_one_tag, NULL);
      // g_print ("\n");
      // gst_tag_list_unref (tags);
      break;
    default:
      /* We should not reach here */
      // g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
      // g_printerr ("Unexpected message received.\n");
      break;
  }

  return TRUE;
}

/**
 * @brief: 修改jitterbuffer元素往后的所有元素接受到的Caps
 *         删除 "seqnum-base"
 */
GstPadProbeReturn  
cb_jitterbuffer_sink_pad_probe_cb (GstPad *pad, 
                                   GstPadProbeInfo *info,
                                   gpointer user_data) {
                      
  GstEvent *event = gst_pad_probe_info_get_event (info);

  // g_print ("%s\n", __func__);

  if (GST_EVENT_TYPE (event) == GST_EVENT_CAPS) {
     GstCaps *caps;
     gst_event_parse_caps (event, &caps);
     GstStructure *structure = gst_caps_get_structure (caps, 0);
     gst_structure_remove_field (structure, "seqnum-base");
    //  gst_structure_set (structure, "seqnum-base", 50, NULL);
  }

  return GST_PAD_PROBE_OK;
}


static void
cb_rtspsrc_get_jitterbuffer (GstElement * object,
                             GstElement * jitterbuffer,
                             gpointer user_data) {
  GstPad *sinkpad = gst_element_get_static_pad (jitterbuffer, "sink");
  gst_pad_add_probe (sinkpad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, cb_jitterbuffer_sink_pad_probe_cb, NULL, NULL);
  gst_object_unref (sinkpad);
}

static void 
cb_rtspsrc_new_manager (GstElement * object,
                        GstElement * manager,
                        gpointer user_data) {
  g_signal_connect(manager, "new-jitterbuffer", G_CALLBACK(cb_rtspsrc_get_jitterbuffer), NULL);
}


static gboolean
timeout_cb (CustomData *data) {

  GstStateChangeReturn ret = gst_element_set_state (data->filesink, GST_STATE_PAUSED);
  g_print ("ret = %d\n", ret);

  return FALSE;
}


int
main (int argc, char *argv[]) {

  CustomData data;
  GstStateChangeReturn ret;

  // g_setenv("GST_DEBUG_DUMP_DOT_DIR", "./", TRUE);

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  data.source = gst_element_factory_make ("rtspsrc", "rtsp source");

  /* 视频 */
  data.h265depay = gst_element_factory_make ("rtph265depay", "rtph265depay");
  data.h265parse = gst_element_factory_make ("h265parse", "h265parse");
  data.h265parse_tee = gst_element_factory_make ("tee", "h265parse_tee");
  data.h265decode = gst_element_factory_make ("nvv4l2decoder", "nvv4l2decoder"); // nvv4l2decoder avdec_h265
  data.video_convert = gst_element_factory_make ("nvvideoconvert", "videoconvert");
  data.video_sink = gst_element_factory_make ("nveglglessink", "video_sink"); //nv3dsink

  /* 音频 */
  data.rtpmp4gdepay = gst_element_factory_make ("rtpmp4gdepay", "rtpmp4gdepay");
  data.aacparse = gst_element_factory_make ("aacparse", "aacparse");
  data.aacparse_tee = gst_element_factory_make ("tee", "aacparse_tee");
  data.avdec_aac = gst_element_factory_make ("avdec_aac", "avdec_aac");
  data.audio_convert = gst_element_factory_make ("audioconvert", "audioconvert");
  data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  /* 录像 */
  data.h265_parse_filter = gst_element_factory_make ("capsfilter", "h265_parse_filter");
  data.record_h265_parse = gst_element_factory_make ("h265parse", "record_h265_parse");
  data.qtmux = gst_element_factory_make ("qtmux", "qtmux");
  data.filesink = gst_element_factory_make ("filesink", "filesink");

  data.pipeline = gst_pipeline_new ("test-pipeline");

  data.loop = g_main_loop_new (NULL, FALSE);

  if (!data.pipeline || !data.source ||
      !data.h265depay || !data.h265parse || !data.h265parse_tee || !data.h265decode || !data.video_convert || !data.video_sink || 
      !data.rtpmp4gdepay || !data.aacparse || !data.aacparse_tee || !data.avdec_aac || !data.audio_convert || !data.audio_sink ||
      !data.record_h265_parse || !data.qtmux || !data.filesink ) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (data.pipeline), data.source, 
      data.h265depay, data.h265parse, data.h265parse_tee, data.h265decode, data.video_convert, data.video_sink, \
      data.rtpmp4gdepay, data.aacparse, data.aacparse_tee, data.avdec_aac, data.audio_convert, data.audio_sink, \
      data.record_h265_parse, data.qtmux, data.filesink, NULL);


  /* 延迟设小了会卡，跟nvv4l2decoder有关 */
  g_object_set(data.source, "location", "rtsp://admin:yangquan123@192.168.10.14:554/Streaming/Channels/101", \
                          "latency", 300, "protocols", 0x04, NULL);

  g_object_set (data.video_sink, "window-width", 640, "window-height", 480, NULL);

  g_object_set (data.filesink, "location", "video%02d.mkv", "sync", FALSE, NULL);

  g_object_set (data.pipeline, "async-handling", TRUE, NULL);

  GstStructure *structure = gst_structure_new_from_string ("properties,sync=false,async=false");

  /* 如果不设定bin异步处理状态，如果在读取流失败的状态下，设定管道为运行状态，则会阻塞整个管道 */
  // g_object_set (G_OBJECT (data.splitmuxsink), "max-size-time", GST_SECOND * 60, "max-files", 20, "sink-properties", structure,\
  //     "async-finalize", TRUE, "async-handling", TRUE, "message-forward", TRUE, NULL);

  /* 在这里把回调函数的src data变量指定参数*/
  g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), &data);
  g_signal_connect (data.source, "new-manager", G_CALLBACK(cb_rtspsrc_new_manager), NULL);


  if (!gst_element_link_many (data.h265depay, data.h265parse, data.h265parse_tee, NULL)) {
    g_printerr ("h265depay and h265parse Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.h265decode, data.video_convert, data.video_sink, NULL)) {
    g_printerr ("Video Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_pads(data.h265parse_tee, "src_%u", data.h265decode, "sink")) {
    g_printerr ("h265parse_tee and h265decode Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.rtpmp4gdepay, data.aacparse, data.aacparse_tee, NULL)) {
    g_printerr ("rtpmp4gdepay and aacparse Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.avdec_aac, data.audio_convert, data.audio_sink, NULL)) {
    g_printerr ("Audio Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_pads(data.aacparse_tee, "src_%u", data.avdec_aac, "sink")) {
    g_printerr ("aacparse_tee and avdec_aac Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  /* 连接录像 */
  // if (!gst_element_link_pads(data.h265parse_tee, "src_%u", data.record_h265_parse, "sink")) {
  //   g_printerr ("h265parse_tee and record_h265_parse Elements could not be linked.\n");
  //   gst_object_unref (data.pipeline);
  //   return -1;
  // }

  if (!gst_element_link_pads(data.record_h265_parse, "src", data.qtmux, "video_%u")) {
    g_printerr ("record_h265_parse and splitmuxsink Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_pads(data.qtmux, "src", data.filesink, "sink")) {
    g_printerr ("qtmux and filesink Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }
  
  /* Listen to the bus */
  GstBus *bus = gst_element_get_bus (data.pipeline);
  guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, &data);
  gst_object_unref (bus);

  /* 设定管道为播放状态 */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  g_print ("ret = %d\n", ret);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  g_timeout_add_seconds (5, G_SOURCE_FUNC(timeout_cb), &data);

  g_main_loop_run (data.loop);

  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  g_source_remove (bus_watch_id);
  gst_object_unref (data.pipeline);
  g_main_loop_unref (data.loop);

  return 0;
}
