#include <gtk/gtk.h>
#include <gst/gst.h>

GdkPaintable *picture_paintable = NULL;
GtkWidget *picture = NULL;

static void
ui_render_cb (GstElement *element, GdkPaintable *paintable, gpointer data){
  gtk_picture_set_paintable (GTK_PICTURE(picture), paintable);
}

typedef struct _CustomData
{
  GstElement *pipeline;
  GstElement *source;

  GstElement *h264depay;
  GstElement *h264parse;
  GstElement *queue;
  GstElement *decode;
  GstElement *convert;
  GstElement *filter;
  GstElement *sink;

} CustomData;

static void
pad_added_handler (GstElement * src, GstPad * new_pad, CustomData * data)
{
  GstPad *sink_pad = NULL;
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;  
  
  new_pad_caps = gst_pad_get_current_caps (new_pad);
  /* 通过gst_caps_get_size函数可以的出Caps中有几个GstStructure */
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);

  g_message("%s type is %s\n", GST_PAD_NAME (new_pad), new_pad_type);
  // gst_structure_foreach (new_pad_struct, print_pad_structure, NULL);

  sink_pad = gst_element_get_static_pad (data->h264depay, "sink");

  // static int i = 0;
  // if (i++ == 0)
  //   sink_pad = gst_element_get_static_pad (data->h264depay, "sink");
  // else 
  //   sink_pad = gst_element_get_static_pad (data->h264depay_t, "sink");

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

  /* Unreference the sink pad */
  gst_object_unref (sink_pad);
}

static gpointer
play_video (gpointer use_data) {

  CustomData data;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  gst_init (NULL, NULL);

  /* Create the elements */
  data.source = gst_element_factory_make ("rtspsrc", "source");

  data.h264depay = gst_element_factory_make ("rtph264depay", "rtph264depay");
  data.h264parse = gst_element_factory_make ("h264parse", "h264parse");
  data.queue = gst_element_factory_make ("queue", "queue");
  data.decode = gst_element_factory_make ("nvv4l2decoder", "nvv4l2decoder"); // nvv4l2decoder avdec_h264
  data.filter = gst_element_factory_make("capsfilter", "filter");
  data.convert = gst_element_factory_make ("nvvideoconvert", "videoconvert");

  data.sink = gst_element_factory_make ("vpfeglglessink", "sink"); //nv3dsink

  g_signal_connect (data.sink, "paintable", G_CALLBACK(ui_render_cb), NULL);

  GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                      "format", G_TYPE_STRING, "RGBA",
                                      NULL);
  GstCapsFeatures *feature = gst_caps_features_new ("memory:NVMM", NULL);
  gst_caps_set_features (caps, 0, feature);

  // 设置Caps到filter
  g_object_set(G_OBJECT(data.filter), "caps", caps, NULL);
  gst_caps_unref(caps);

  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");

  if (!data.pipeline || !data.source || !data.h264depay || !data.h264parse || !data.queue \
      || !data.decode || !data.convert || !data.filter || !data.sink) {
    g_printerr ("Not all elements could be created.\n");
    return NULL;
  }

  /* Build the pipeline. Note that we are NOT linking the source at this
   * point. We will do it later. */
  gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.h264depay, data.queue, \
      data.h264parse, data.decode, data.convert, data.filter, data.sink, NULL);


  if (!gst_element_link_many (data.h264depay, data.h264parse, data.queue, \
                              data.decode, data.convert, data.filter, data.sink, NULL)) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return NULL;
  }

  /* Set the URI to play */
  g_object_set(data.source, "location", "rtsp://admin:YEERBA@192.168.10.11:554/Streaming/Channels/101", \
                            "latency", 200, "protocols", 0x04, NULL);
  

  // g_object_set(data.source, "location", "rtsp://admin:LHLQLW@192.168.10.199:554/Streaming/Channels/101", 
  //                           "latency", 200, "protocols", 0x04, NULL); // 家客厅

  /* Connect to the pad-added signal */
  /* 在这里把回调函数的src data变量指定参数*/
  g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler),
      &data);
  
  /* Start playing */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return NULL;
  }

  /*GstPad* pad = gst_element_get_static_pad (data.convert, "sink");
  gst_pad_set_event_function(pad,gst_event_callback);*/
  
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
app_activate (GApplication *app, gpointer *user_data) {

  g_thread_try_new ("gst.play", play_video, NULL, NULL);

  // play_video (NULL);

  GtkWidget *win = gtk_application_window_new (GTK_APPLICATION (app));

  gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

  gtk_widget_set_size_request (win, 500, 400);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *button = gtk_button_new_with_label ("按钮");
  GtkWidget *label = gtk_label_new ("标签");

  // GtkWidget *image = gtk_image_new_from_paintable (nuclear); picture可以不受长宽比拉伸

  picture = gtk_picture_new ();

  gtk_widget_set_hexpand (picture, TRUE);
  gtk_widget_set_vexpand (picture, TRUE);

  // gtk_box_append (GTK_BOX(box), label);
  gtk_box_append (GTK_BOX(box), picture);
  // gtk_box_append (GTK_BOX(box), button);

  gtk_window_set_child (GTK_WINDOW(win), GTK_WIDGET(box));

  // gtk_widget_set_opacity (win, 0.75);

  gtk_window_present (GTK_WINDOW (win));
}

int
main (int argc, char *argv[]) {

  GtkApplication *app = gtk_application_new ("test.application.Paintable", G_APPLICATION_DEFAULT_FLAGS);
  
  g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
  g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return 0;
}
