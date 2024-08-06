#include <gst/gst.h>

/* 这里存下了所有需要的局部变量，因为本教程中会有回调函数，使用struct比较方便 */
typedef struct _CustomData
{
  GstElement *pipeline;
  GstElement *source;

  GstElement *h264depay;
  GstElement *h264parse;
  GstElement *tee;

  GstElement *h264decode;
  GstElement *video_convert;
  GstElement *video_sink;

  GstElement *qtmux;
  GstElement *filesink;

} CustomData;


static void pad_added_handler (GstElement * src, GstPad * pad,
    CustomData * data);


/**
 * @brief: 修改jitterbuffer元素往后的所有元素接受到的Caps
 *         删除 "seqnum-base"
 */
GstPadProbeReturn  
cb_jitterbuffer_sink_pad_probe_cb (GstPad *pad, 
                                   GstPadProbeInfo *info,
                                   gpointer user_data) {
                      
  GstEvent *event = gst_pad_probe_info_get_event (info);

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
  data.tee = gst_element_factory_make ("tee", "tee");

  data.h264decode = gst_element_factory_make ("avdec_h264", "nvv4l2decoder"); // nvv4l2decoder avdec_h264
  data.video_convert = gst_element_factory_make ("nvvideoconvert", "videoconvert");
  data.video_sink = gst_element_factory_make ("nveglglessink", "video_sink"); //nv3dsink

  
  data.qtmux = gst_element_factory_make ("qtmux", "qtmux");
  data.filesink = gst_element_factory_make ("filesink", "filesink");


  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");

  if (!data.pipeline || !data.source || !data.h264depay || !data.h264parse  || !data.tee
      || !data.h264decode || !data.video_convert || !data.video_sink \
      || !data.qtmux || !data.filesink ) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (data.pipeline), data.source,  data.h264depay, data.h264parse, data.tee, \
                                             data.h264decode, data.video_convert, data.video_sink, \
                                             data.qtmux, data.filesink, NULL);

  if (!gst_element_link_many (data.h264depay, data.h264parse, data.tee, NULL)) {
    g_printerr ("Video Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.h264decode, data.video_convert, data.video_sink, NULL)) {
    g_printerr ("Video Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  if (!gst_element_link_many (data.qtmux, data.filesink, NULL)) {
    g_printerr ("Video Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }


  GstPad *decode_src_pad = gst_element_request_pad_simple (data.tee, "src_%u");
  GstPad *sink_pad = gst_element_get_static_pad (data.h264decode, "sink");
  if (decode_src_pad && sink_pad) {
    GstPadLinkReturn ret = gst_pad_link (decode_src_pad, sink_pad);
    if (ret == GST_PAD_LINK_OK)
      g_print ("Link Success\n");
    else 
      g_print ("Link failed\n");
    gst_object_unref (decode_src_pad);
    gst_object_unref (sink_pad);
  } else {
    g_print ("not crate pad\n");
  }

  GstPad *file_record_src_pad = gst_element_request_pad_simple (data.tee, "src_%u");
  GstPad *qtmux_video_sink_pad = gst_element_request_pad_simple (data.qtmux, "video_%u");
  if (file_record_src_pad && qtmux_video_sink_pad) {
    GstPadLinkReturn ret = gst_pad_link (file_record_src_pad, qtmux_video_sink_pad);
    if (ret == GST_PAD_LINK_OK)
      g_print ("Link Success\n");
    else 
      g_print ("Link failed\n");
    gst_object_unref (file_record_src_pad);
    gst_object_unref (qtmux_video_sink_pad);
  } else {
    g_print ("not crate pad\n");
  }


  g_signal_connect (data.source, "new-manager", G_CALLBACK(cb_rtspsrc_new_manager), NULL);

  /* 延迟设小了会卡，跟nvv4l2decoder有关 */
  g_object_set(data.source, "location", "rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101", \
                          "latency", 0, "protocols", 0x04, NULL);
  
  // g_object_set(data.source, "location", "rtsp://admin:LHLQLW@192.168.10.199:554/Streaming/Channels/101", 
  //                           "latency", 200, "protocols", 0x04, NULL); // 家客厅

  g_object_set (data.filesink, "location", "test.mp4", NULL);

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
      
      if (!g_strcmp0 (media_type, "video")) {
        
        sink_pad = gst_element_get_static_pad (data->h264depay, "sink");
        
        g_print ("sink_pad = %p\n", sink_pad);
      
      } else 
        goto exit;


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
