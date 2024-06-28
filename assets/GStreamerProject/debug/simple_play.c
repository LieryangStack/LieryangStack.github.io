#include <gst/gst.h>

int main(int argc, char *argv[]) {

  GstElement *pipeline, *source, *convert, *sink;
  GstBus *bus;
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init(&argc, &argv);

  /* Create the elements */
  source = gst_element_factory_make("videotestsrc", "source");
  convert = gst_element_factory_make("videoconvert", "convert");
  sink = gst_element_factory_make("ximagesink", "sink");

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new("test-pipeline");

  if (!pipeline || !source || !convert || !sink) {
      g_printerr("Not all elements could be created.\n");
      return -1;
  }

  /* Build the pipeline */
  gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);
  if (gst_element_link_many(source, convert, sink, NULL) != TRUE) {
      g_printerr("Elements could not be linked.\n");
      gst_object_unref(pipeline);
      return -1;
  }

  /* Start playing */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Free resources */
  if (msg != NULL)
      gst_message_unref(msg);
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}
