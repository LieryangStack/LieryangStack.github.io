#include <gst/gst.h>
#include <glib.h>
#include <signal.h>

// Global variable for the main loop
static GMainLoop *loop;

// Signal handler function for SIGINT
static void handle_sigint(int sig) {
    g_print("Caught SIGINT, stopping main loop...\n");
    g_main_loop_quit(loop);
}

// Callback function to handle the "pad-added" signal from the rtspsrc element
static void on_pad_added(GstElement *src, GstPad *new_pad, GstElement *depay) {
    GstPad *sink_pad = gst_element_get_static_pad(depay, "sink");

    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    // Try to link the newly added pad to the depayloader sink pad
    if (gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK) {
        g_printerr("Failed to link pads.\n");
    } else {
        g_print("Pads linked successfully.\n");
    }

    g_object_unref(sink_pad);
}

// Callback function to handle error messages
static void on_error(GstBus *bus, GstMessage *msg, GMainLoop *loop) {
    GError *err;
    gchar *debug_info;

    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");

    g_clear_error(&err);
    g_free(debug_info);

    g_main_loop_quit(loop);
}

// Callback function to handle EOS (end of stream) messages
static void on_eos(GstBus *bus, GstMessage *msg, GMainLoop *loop) {
    g_print("End-Of-Stream reached.\n");
    g_main_loop_quit(loop);
}


static GstPadProbeReturn
src_bin_buf_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data) {

  if ((info->type & GST_PAD_PROBE_TYPE_BUFFER) == GST_PAD_PROBE_TYPE_BUFFER) {
    GstBuffer *buffer = GST_BUFFER (info->data);

    // g_print ("%s\n", (gchar *)u_data);
    // g_print ("pts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->pts));
    // g_print ("dts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->dts));
    // g_print ("duration: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(buffer->duration));

    // if (!g_strcmp0((const gchar *)u_data, "sink_0")) {
    //   g_print ("%s\n", (gchar *)u_data);
    //   g_print ("pts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->pts));
    //   g_print ("dts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->dts));
    //   g_print ("duration: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(buffer->duration));
    // }

    if (!GST_CLOCK_TIME_IS_VALID (buffer->pts)) {
      if (!GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_DELTA_UNIT))
        g_print ("GST_BUFFER_FLAG_DELTA_UNIT\n");
      g_print ("pts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->pts));
      g_print ("dts: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(buffer->dts));
      g_print ("duration: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(buffer->duration));
    }
  }

  return GST_PAD_PROBE_OK;
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


int main(int argc, char *argv[]) {
    GstElement *pipeline, *rtspsrc, *depay, *parse, *h265timestamper, *mux, *sink;
    GstBus *bus;
    GstMessage *msg;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create the elements
    rtspsrc = gst_element_factory_make("rtspsrc", "source");
    depay = gst_element_factory_make("rtph265depay", "depay");
    parse = gst_element_factory_make("h265parse", "parse");
    h265timestamper = gst_element_factory_make("h265timestamper", "h265timestamper");
    mux = gst_element_factory_make("qtmux", "mux"); /* matroskamux */
    sink = gst_element_factory_make("filesink", "sink");

    // Create the empty pipeline
    pipeline = gst_pipeline_new("rtsp-pipeline");

    if (!pipeline || !rtspsrc || !depay || !parse || !mux || !sink || !h265timestamper) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_signal_connect (rtspsrc, "new-manager", G_CALLBACK(cb_rtspsrc_new_manager), NULL);

    // Set the element properties
    g_object_set(rtspsrc, "location", "rtsp://admin:yangquan123@192.168.10.14:554/Streaming/Channels/101", NULL);
    g_object_set(rtspsrc, "protocols", 0x04, NULL);  // Set protocols to 0x04 (TCP)
    g_object_set(sink, "location", "received_h265.mkv", NULL);

    GstPad *src_pad = gst_element_get_static_pad (parse, "src");
    GstPad *sink_pad = gst_element_get_static_pad (parse, "sink");
    gst_pad_add_probe (src_pad, GST_PAD_PROBE_TYPE_BUFFER,
        src_bin_buf_probe, NULL, NULL);
    // gst_pad_add_probe (sink_pad, GST_PAD_PROBE_TYPE_BUFFER,
    //     sink_bin_buf_probe, g_strdup_printf ("camera 0%d(parser sink_pad)", bin->bin_id) , g_free);
    gst_object_unref (src_pad);
    gst_object_unref (sink_pad);

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), rtspsrc, depay, parse, h265timestamper, mux, sink, NULL);

    // Link the elements together. Note that rtspsrc has "pad-added" signal.
    if (!gst_element_link_many(depay, parse, h265timestamper, mux, sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Connect to the pad-added signal for rtspsrc
    g_signal_connect(rtspsrc, "pad-added", G_CALLBACK(on_pad_added), depay);

    // Register the SIGINT signal handler
    signal(SIGINT, handle_sigint);

    // Start playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Create and run the main loop
    loop = g_main_loop_new(NULL, FALSE);
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message::error", G_CALLBACK(on_error), loop);
    g_signal_connect(bus, "message::eos", G_CALLBACK(on_eos), loop);

    g_main_loop_run(loop);

    GstEvent *eos_event = gst_event_new_eos ();
    gst_element_send_event (pipeline, eos_event);

    g_usleep (1000);

    // Free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}

