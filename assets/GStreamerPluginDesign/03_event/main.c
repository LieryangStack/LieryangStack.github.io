#include <gst/gst.h>

static gboolean
bus_call (GstBus     *bus,
      GstMessage *msg,
      gpointer    data)
{
  GMainLoop *loop = data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print ("End-of-stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar *debug = NULL;
      GError *err = NULL;

      gst_message_parse_error (msg, &err, &debug);

      g_print ("Error: %s\n", err->message);
      g_error_free (err);

      if (debug) {
        g_print ("Debug details: %s\n", debug);
        g_free (debug);
      }

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

static gboolean
timeout_cb (gpointer user_data) {

  GstElement *pipeline = GST_ELEMENT (user_data);
  
  GstEvent *event = gst_event_new_flush_start ();

  gst_element_send_event (pipeline, event);

  g_usleep (G_USEC_PER_SEC * 1);

  event = gst_event_new_flush_stop (TRUE);

  gst_element_send_event (pipeline, event);

  return FALSE;
}

gint
main (gint argc, gchar *argv[]) {
  
  GstStateChangeReturn ret;
  GstElement *pipeline, *filesrc, *filter, *sink;
  GstElement *convert;
  GMainLoop *loop;
  GstBus *bus;
  guint watch_id;

  /* initialization */
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);

  /* create elements */
  pipeline = gst_pipeline_new ("pipeline");

  /* watch for messages on the pipeline's bus (note that this will only
   * work like this when a GLib main loop is running) */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  filesrc  = gst_element_factory_make ("audiotestsrc", "my_filesource");

  /* use "identity" here for a filter that does nothing */
  filter   = gst_element_factory_make ("my_filter", "my_filter");

  convert  = gst_element_factory_make ("audioconvert", "convert");

  sink     = gst_element_factory_make ("autoaudiosink", "audiosink");

  if (!sink || !filter || !convert || !sink) {
    g_print ("Decoder or output could not be found - check your install\n");
    return -1;
  }

  gst_bin_add_many (GST_BIN (pipeline), filesrc, filter, convert, sink,  NULL);

  /* link everything together */
  if (!gst_element_link_many (filesrc, filter, convert, sink, NULL)) {
    g_print ("Failed to link one or more elements!\n");
    return -1;
  }

  /* run */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GstMessage *msg;

    g_print ("Failed to start up pipeline!\n");

    /* check if there is an error message with details on the bus */
    msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
    if (msg) {
      GError *err = NULL;

      gst_message_parse_error (msg, &err, NULL);
      g_print ("ERROR: %s\n", err->message);
      g_error_free (err);
      gst_message_unref (msg);
    }
    return -1;
  }

  g_timeout_add_seconds (1, timeout_cb, pipeline);

  g_main_loop_run (loop);

  /* clean up */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_source_remove (watch_id);
  g_main_loop_unref (loop);

  return 0;
}