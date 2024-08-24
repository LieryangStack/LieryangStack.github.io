#include <gst/gst.h>

int main(int argc, char *argv[]) {
  GstElement *pipeline, *source, *tee, *queue1, *queue2, *sink1, *sink2;
  GstBus *bus;
  GstMessage *msg;
  GstPad *tee_src_pad1, *tee_src_pad2;
  GstPad *queue1_pad, *queue2_pad;

  // 初始化GStreamer
  gst_init(&argc, &argv);

  // 创建GStreamer元素
  pipeline = gst_pipeline_new("test-pipeline");
  source = gst_element_factory_make("videotestsrc", "source");
  tee = gst_element_factory_make("tee", "tee");
  queue1 = gst_element_factory_make("queue", "queue1");
  queue2 = gst_element_factory_make("queue", "queue2");
  sink1 = gst_element_factory_make("ximagesink", "sink1");
  sink2 = gst_element_factory_make("ximagesink", "sink2");

  if (!pipeline || !source || !tee || !queue1 || !queue2 || !sink1 || !sink2) {
      g_printerr("Failed to create elements.\n");
      return -1;
  }

  // 将元素添加到管道中
  gst_bin_add_many(GST_BIN(pipeline), source, tee, queue1, queue2, sink1, sink2, NULL);

  // g_object_set (sink1, "sync", FALSE, NULL);

  // g_object_set (sink2, "sync", FALSE, NULL);

  // 连接元素
  if (!gst_element_link(source, tee)) {
      g_printerr("Failed to link source to tee.\n");
      return -1;
  }

  if (!gst_element_link_many(queue1, sink1, NULL)) {
      g_printerr("Failed to link queue1 to sink1.\n");
      return -1;
  }

  if (!gst_element_link_many(queue2, sink2, NULL)) {
      g_printerr("Failed to link queue2 to sink2.\n");
      return -1;
  }

  // 获取 tee 的两个源 pad，并分别链接到两个队列的输入 pad
  tee_src_pad1 = gst_element_request_pad_simple(tee, "src_%u");
  queue1_pad = gst_element_get_static_pad(queue1, "sink");
  tee_src_pad2 = gst_element_request_pad_simple(tee, "src_%u");
  queue2_pad = gst_element_get_static_pad(queue2, "sink");

  if (gst_pad_link(tee_src_pad1, queue1_pad) != GST_PAD_LINK_OK ) {
      g_printerr("Failed to link tee to queues.\n");
      return -1;
  }

  // if (gst_pad_link(tee_src_pad2, queue2_pad) != GST_PAD_LINK_OK ) {
  //     g_printerr("Failed to link tee to queues.\n");
  //     return -1;
  // }

  gst_object_unref(queue1_pad);
  gst_object_unref(queue2_pad);
  gst_object_unref(tee_src_pad1);
  gst_object_unref(tee_src_pad2);

  // 设置管道状态为播放
  
  // g_print ("\n\n");
  // GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_READY); g_print ("ret = %d\n\n", ret);
  // ret = gst_element_set_state(pipeline, GST_STATE_PLAYING); g_print ("ret = %d\n\n", ret);

  // g_usleep (1000);

  /* 这可以解决 管道中有其他未连接的sink导致管道暂停 */
  gst_element_set_state (source, GST_STATE_PLAYING);
  gst_element_set_state (tee, GST_STATE_PLAYING);
  gst_element_set_state (queue1, GST_STATE_PLAYING);
  gst_element_set_state (sink1, GST_STATE_PLAYING);

  GstState state, pending;
  gst_element_get_state (GST_ELEMENT (pipeline), &state, &pending, 0);
  g_print ("state = %d, pending = %d\n", state, pending);

  // 等待直到错误或EOS
  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                    GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  // 处理消息
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
      case GST_MESSAGE_ERROR:
          gst_message_parse_error(msg, &err, &debug_info);
          g_printerr("Error received from element %s: %s\n",
                      GST_OBJECT_NAME(msg->src), err->message);
          g_printerr("Debugging information: %s\n",
                      debug_info ? debug_info : "none");
          g_clear_error(&err);
          g_free(debug_info);
          break;
      case GST_MESSAGE_EOS:
          g_print("End-Of-Stream reached.\n");
          break;
      default:
          g_printerr("Unexpected message received.\n");
          break;
    }
    gst_message_unref(msg);
  }

  // 释放资源
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}
