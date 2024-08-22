#include <gst/gst.h>

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
              G_GUINT64_FORMAT "\"/>",
              segment->flags, segment->rate, segment->applied_rate,
              segment->format, segment->base, segment->offset, segment->start,
              segment->stop, segment->time, segment->position,
              segment->duration);
      }
    }

  }

  return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
GstElement *pipeline, *source, *parser, *decoder, *video_sink;
GstBus *bus;
GstMessage *msg;
gboolean terminate = FALSE;

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

// 开始播放
gst_element_set_state(pipeline, GST_STATE_PLAYING);




// 监听消息总线
bus = gst_element_get_bus(pipeline);
do {
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
      case GST_MESSAGE_ERROR:
      GstFormat format = GST_FORMAT_TIME;
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);
        terminate = TRUE;
        break;
      case GST_MESSAGE_EOS:
        g_print("End-Of-Stream reached.\n");
        terminate = TRUE;
        break;
      case GST_MESSAGE_STATE_CHANGED:
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
            g_print("Pipeline state changed from %s to %s:\n",
                    gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
        }
        break;
      default:
        // Unhandled message
        break;
    }
    gst_message_unref(msg);
  }
} while (!terminate);

// 清理
gst_object_unref(bus);
gst_element_set_state(pipeline, GST_STATE_NULL);
gst_object_unref(pipeline);

return 0;
}
