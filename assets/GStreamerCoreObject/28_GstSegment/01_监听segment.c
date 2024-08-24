/*

gst-launch-1.0 filesrc location=sample_720p.mp4 ! \
qtdemux name=demux \
demux.video_0 ! queue ! h264parse ! avdec_h264 ! videoconvert ! ximagesink \
demux.audio_0 ! queue ! aacparse ! avdec_aac ! audioconvert ! pulsesink

*/

#include <gst/gst.h>
#include <gst/gstsegment.h>
#include <gst/base/gstbasesink.h>

static void 
on_video_pad_added(GstElement *src, GstPad *new_pad, gpointer data) {
    GstPad *sink_pad = gst_element_get_static_pad(data, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);

    if (g_str_has_prefix(new_pad_type, "video/x-h264")) {
        /* It is video */
        if (gst_pad_is_linked(sink_pad)) {
            g_print("We are already linked. Ignoring.\n");
            goto exit;
        }
        /* Attempt the link */
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_printerr("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print("Link succeeded (type '%s').\n", new_pad_type);
        }
    } else {
        g_print("It has type '%s' which is not raw audio/video. Ignoring.\n", new_pad_type);
    }

exit:
    if (new_pad_caps != NULL)
        gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}

static void 
on_audio_pad_added(GstElement *src, GstPad *new_pad, gpointer data) {
    GstPad *sink_pad = gst_element_get_static_pad(data, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);

    if (g_str_has_prefix(new_pad_type, "audio/mpeg")) {
        /* It is audio */
        if (gst_pad_is_linked(sink_pad)) {
            g_print("We are already linked. Ignoring.\n");
            goto exit;
        }
        /* Attempt the link */
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_printerr("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print("Link succeeded (type '%s').\n", new_pad_type);
        }
    } else {
        g_print("It has type '%s' which is not raw audio/video. Ignoring.\n", new_pad_type);
    }

exit:
    if (new_pad_caps != NULL)
        gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}


static GstPadProbeReturn 
on_pad_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {

  const GstSegment *segment;

  if (GST_PAD_PROBE_INFO_TYPE (info) & (GST_PAD_PROBE_TYPE_EVENT_BOTH | GST_PAD_PROBE_TYPE_EVENT_FLUSH)) {
    GstEvent *event = GST_PAD_PROBE_INFO_EVENT (info);
    // g_print ("event %p %s\n", event, GST_EVENT_TYPE_NAME (event));

    switch (GST_EVENT_TYPE (event)) {
      case GST_EVENT_SEGMENT: {

        gst_event_parse_segment (event, &segment);
        g_print("\nstop time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(segment->stop));
        g_print("position time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(segment->position));
        g_print("start time: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(segment->start));

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



static gboolean
timeout_cb (gpointer data) {

  GstBaseSink *element = (GstBaseSink *)data;

  GstSegment *segment = &element->segment;

  g_print("\nstop time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(segment->stop));
  g_print("position time: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(segment->position));
  g_print("start time: %" GST_TIME_FORMAT "\n\n", GST_TIME_ARGS(segment->start));

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



  return TRUE;
}

int 
main(int argc, char *argv[]) {

    GstElement *pipeline, *filesrc, *qtdemux, *video_queue, *audio_queue, *h264parse, *avdec_h264, *videoconvert, *ximagesink, *aacparse, *avdec_aac, *audioconvert, *pulsesink;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    filesrc = gst_element_factory_make("filesrc", "source");
    qtdemux = gst_element_factory_make("qtdemux", "demuxer");
    video_queue = gst_element_factory_make("queue", "video_queue");
    audio_queue = gst_element_factory_make("queue", "audio_queue");
    h264parse = gst_element_factory_make("h264parse", "h264parser");
    avdec_h264 = gst_element_factory_make("avdec_h264", "h264decoder");
    videoconvert = gst_element_factory_make("videoconvert", "converter");
    ximagesink = gst_element_factory_make("ximagesink", "videosink");
    aacparse = gst_element_factory_make("aacparse", "aacparser");
    avdec_aac = gst_element_factory_make("avdec_aac", "aacdecoder");
    audioconvert = gst_element_factory_make("audioconvert", "audioconverter");
    pulsesink = gst_element_factory_make("pulsesink", "audiosink");

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("video-audio-player");

    if (!pipeline || !filesrc || !qtdemux || !video_queue || !audio_queue || !h264parse || !avdec_h264 || !videoconvert || !ximagesink || !aacparse || !avdec_aac || !audioconvert || !pulsesink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Set the input file location */
    g_object_set(filesrc, "location", "video/sample_720p.mp4", NULL);

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), filesrc, qtdemux, video_queue, audio_queue, h264parse, avdec_h264, videoconvert, ximagesink, aacparse, avdec_aac, audioconvert, pulsesink, NULL);

    /* Link the elements that can be linked immediately */
    if (!gst_element_link(filesrc, qtdemux)) {
        g_printerr("Filesrc and demuxer could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link_many(video_queue, h264parse, avdec_h264, videoconvert, ximagesink, NULL)) {
        g_printerr("Video elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    if (!gst_element_link_many(audio_queue, aacparse, avdec_aac, audioconvert, pulsesink, NULL)) {
        g_printerr("Audio elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    GstPad *sink_pad = gst_element_get_static_pad(ximagesink, "sink");
    gst_pad_add_probe(sink_pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BUFFER, on_pad_probe, NULL, NULL);
    gst_object_unref(sink_pad);


    /* Connect the demuxer's pad-added signal */
    g_signal_connect(qtdemux, "pad-added", G_CALLBACK(on_video_pad_added), video_queue);
    g_signal_connect(qtdemux, "pad-added", G_CALLBACK(on_audio_pad_added), audio_queue);



    loop = g_main_loop_new (NULL, FALSE);

    GstBus *bus = gst_element_get_bus (pipeline);
    guint bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, pipeline);
    gst_object_unref (bus);

    /* 开始播放 */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_timeout_add_seconds (2, G_SOURCE_FUNC(timeout_cb), ximagesink);

    g_main_loop_run (loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref (loop);

    return 0;
}

