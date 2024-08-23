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


// 处理所有事件的函数
static GstPadProbeReturn 
on_pad_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {

  if (GST_PAD_PROBE_INFO_TYPE (info) & (GST_PAD_PROBE_TYPE_EVENT_BOTH | GST_PAD_PROBE_TYPE_EVENT_FLUSH)) {
    GstEvent *event = GST_PAD_PROBE_INFO_EVENT (info);
    // g_print ("event %p %s\n", event, GST_EVENT_TYPE_NAME (event));

    switch (GST_EVENT_TYPE (event)) {
      case GST_EVENT_SEGMENT: {
        const GstSegment *segment;
        gst_event_parse_segment (event, &segment);
        g_print("Current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(segment->stop));

        g_print(" flags=\"%d\" rate=\"%f\" applied-rate=\"%f\""
              " format=\"%d\" base=\"%" G_GUINT64_FORMAT "\" offset=\"%"
              G_GUINT64_FORMAT "\" start=\"%" G_GUINT64_FORMAT "\""
              " stop=\"%" G_GUINT64_FORMAT "\" time=\"%" G_GUINT64_FORMAT
              "\" position=\"%" G_GUINT64_FORMAT "\" duration=\"%"
              G_GUINT64_FORMAT "\"\n",
              segment->flags, segment->rate, segment->applied_rate,
              segment->format, segment->base, segment->offset, segment->start,
              segment->stop, segment->time, segment->position,
              segment->duration);
      }
    }

  }

  return GST_PAD_PROBE_OK;
}

static gboolean
timeout_cb (gpointer data) {

  GstElement *pipeline = (GstElement *)data;
  gint64 position = GST_CLOCK_TIME_NONE;
  gint64 duration = GST_CLOCK_TIME_NONE;

  if (gst_element_query_position(pipeline, GST_FORMAT_TIME, &position)) {
      g_print("current running time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(position));
  }

  if (gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration)) {
      g_print("duration time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(duration));
  }

  return FALSE;
}

int 
main(int argc, char *argv[]) {

  GstElement *pipeline, *source, *parser, *decoder, *video_sink;
  gst_init(&argc, &argv);

  // 创建GStreamer元素
  pipeline = gst_pipeline_new("video-player");
  source = gst_element_factory_make("filesrc", "file-source");
  parser = gst_element_factory_make("h264parse", "parser");
  decoder = gst_element_factory_make("avdec_h264", "decoder");
  video_sink = gst_element_factory_make("autovideosink", "video-output");

  if (!pipeline || !source || !parser || !decoder || !video_sink) {
      g_printerr("Not all elements could be created.\n");
      return -1;
  }

  GstPad *sink_pad = gst_element_get_static_pad(video_sink, "sink");
  gst_pad_add_probe(sink_pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BUFFER, on_pad_probe, NULL, NULL);
  gst_object_unref(sink_pad);

  // GstPad *src_pad = gst_element_get_static_pad(parser, "src");
  // gst_pad_add_probe(src_pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BUFFER, on_pad_probe, NULL, NULL);
  // gst_object_unref(src_pad);

  // 设置源文件路径
  g_object_set(G_OBJECT(source), "location", "/opt/nvidia/deepstream/deepstream-6.4/samples/streams/sample_720p.h264", NULL);

  // 将元素添加到管道
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

  // 开始播放
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  g_timeout_add_seconds (1, G_SOURCE_FUNC(timeout_cb), parser);

  g_main_loop_run (loop);

  // 清理
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}
