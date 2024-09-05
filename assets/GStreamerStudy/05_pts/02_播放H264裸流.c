#include <gst/gst.h>

static GstPadProbeReturn
sink_bin_buf_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data) {

  if ((info->type & GST_PAD_PROBE_TYPE_BUFFER) == GST_PAD_PROBE_TYPE_BUFFER) {
    GstBuffer *buffer = GST_BUFFER (info->data);

    g_print ("sink pad\n");
    g_print ("pts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->pts));
    g_print ("dts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->dts));
    g_print ("duration: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(buffer->duration));

  }

  return GST_PAD_PROBE_OK;
}

static GstPadProbeReturn
src_bin_buf_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data) {

  if ((info->type & GST_PAD_PROBE_TYPE_BUFFER) == GST_PAD_PROBE_TYPE_BUFFER) {
    GstBuffer *buffer = GST_BUFFER (info->data);

    g_print ("src pad\n");
    g_print ("pts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->pts));
    g_print ("dts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->dts));
    g_print ("duration: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(buffer->duration));

  }

  return GST_PAD_PROBE_OK;
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *parser, *decoder, *convert, *sink;
    GstBus *bus;
    GstMessage *msg;
    GError *err = NULL;

    /* 初始化 GStreamer */
    gst_init(&argc, &argv);

    /* 创建元素 */
    pipeline = gst_pipeline_new("video-player");
    source = gst_element_factory_make("filesrc", "source");
    parser = gst_element_factory_make("h264parse", "parser");
    decoder = gst_element_factory_make("nvh264dec", "decoder");
    convert = gst_element_factory_make("videoconvert", "convert");
    sink = gst_element_factory_make("ximagesink", "sink");

    if (!pipeline || !source || !parser || !decoder || !convert || !sink) {
        g_printerr("无法创建一个或多个元素.\n");
        return -1;
    }

    /* 设置文件路径 */
    g_object_set(G_OBJECT(source), "location", "sample_720p.h264", NULL);

    // GstPad *src_pad = gst_element_get_static_pad (decoder, "src");
    // GstPad *sink_pad = gst_element_get_static_pad (decoder, "sink");
    // gst_pad_add_probe (sink_pad, GST_PAD_PROBE_TYPE_BUFFER,
    //     sink_bin_buf_probe, NULL, NULL);
    // gst_pad_add_probe (src_pad, GST_PAD_PROBE_TYPE_BUFFER,
    //     src_bin_buf_probe, NULL, NULL);
    // gst_object_unref (src_pad);
    // gst_object_unref (sink_pad);

    /* 构建管道 */
    gst_bin_add_many(GST_BIN(pipeline), source, parser, decoder, convert, sink, NULL);
    if (!gst_element_link_many(source, parser, decoder, convert, sink, NULL)) {
        g_printerr("无法链接元素.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* 启动管道 */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* 监听总线上的消息（例如错误、播放结束等） */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* 处理消息 */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("错误：%s\n", err->message);
            g_error_free(err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("播放结束\n");
            break;
        default:
            break;
        }
        gst_message_unref(msg);
    }

    /* 释放资源 */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
