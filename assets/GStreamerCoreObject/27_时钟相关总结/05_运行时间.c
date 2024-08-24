#include <gst/gst.h>

GMainLoop *loop = NULL;

static gboolean
my_bus_callback (GstBus * bus, GstMessage * message, gpointer user_data) {

  GstElement *pipeline = (GstElement *)user_data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("%s Error: %s\n",GST_OBJECT_NAME (message->src), err->message);
      g_error_free (err);
      g_free (debug);
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "error");

      // g_main_loop_quit (data->loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print ("receive EOS\n");
      g_print("current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(gst_element_get_current_running_time(pipeline)));
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_STATE_CHANGED:
      /* We are only interested in state-changed messages from the pipeline */
      if (GST_MESSAGE_SRC (message) == GST_OBJECT (pipeline)) {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed (message, &old_state, &new_state,
            &pending_state);
        g_message ("Pipeline state changed from %s to %s:\n",
            gst_element_state_get_name (old_state),
            gst_element_state_get_name (new_state));
        char state_name[100];
        g_snprintf (state_name, 100, "%s", gst_element_state_get_name (new_state));
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, state_name);

        g_print ("current time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(gst_element_get_current_clock_time(pipeline)));
        g_print("current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(gst_element_get_current_running_time(pipeline)));
        g_print("pipeline->base_time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(pipeline->base_time));
        g_print("pipeline->start_time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(pipeline->start_time));

      }
      break;
    default:
      /* We should not reach here */
      // g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
      // g_printerr ("Unexpected message received.\n");
      break;
  }

  return TRUE;
}


static gboolean
timeout_cb (gpointer data) {

  g_print ("%s\n", __func__);

  GstElement *pipeline = (GstElement *)data;

  /* 测试 base_time start_time running_time */
  gst_element_set_state(pipeline, GST_STATE_PAUSED);

  g_usleep (2 * G_USEC_PER_SEC);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  

  return FALSE;
}

int 
main(int argc, char *argv[]) {

  GstElement *pipeline, *source, *parser, *decoder, *video_sink;
  gst_init(&argc, &argv);

  /* 创建GStreamer元素 */
  pipeline = gst_pipeline_new("video-player");
  source = gst_element_factory_make("filesrc", "file-source");
  parser = gst_element_factory_make("h264parse", "parser");
  decoder = gst_element_factory_make("avdec_h264", "decoder");
  video_sink = gst_element_factory_make("autovideosink", "video-output");

  if (!pipeline || !source || !parser || !decoder || !video_sink) {
      g_printerr("Not all elements could be created.\n");
      return -1;
  }

  /* 设置源文件路径 */
  g_object_set(G_OBJECT(source), "location", "video/sample_720p.h264", NULL);

  /* 将元素添加到管道 */
  gst_bin_add_many(GST_BIN(pipeline), source, parser, decoder, video_sink, NULL);

  // 链接元素
  if (!gst_element_link_many(source, parser, decoder, video_sink, NULL)) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  loop = g_main_loop_new (NULL, FALSE);

  GstBus *bus = gst_element_get_bus (pipeline);
  guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, pipeline);
  gst_object_unref (bus);

  /* 开始播放 */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  g_timeout_add_seconds (5, G_SOURCE_FUNC(timeout_cb), pipeline);

  g_main_loop_run (loop);

  /* 清理 */
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}
