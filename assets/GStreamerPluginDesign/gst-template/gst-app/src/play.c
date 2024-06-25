#include "play.h"

void
play_uri (const gchar * uri)
{
  GstStateChangeReturn sret;
  GstElement *playbin;
  GstElement *audiosink;
  GstElement *videosink;
  GstMessage *msg = NULL;
  GstBus *bus;

  g_print ("Trying to play %s ...\n", uri);

  playbin = gst_element_factory_make ("playbin", "playbin");
  if (playbin == NULL)
    goto no_playbin;

  /* get playbin's bus - we'll watch it for messages */
  bus = gst_pipeline_get_bus (GST_PIPELINE (playbin));

  /* set audio sink */
  audiosink = gst_element_factory_make ("autoaudiosink", "audiosink");
  if (audiosink == NULL)
    goto no_autoaudiosink;
  g_object_set (playbin, "audio-sink", audiosink, NULL);

  /* set video sink */
  videosink = gst_element_factory_make ("autovideosink", "videosink");
  if (videosink == NULL)
    goto no_autovideosink;
  g_object_set (playbin, "video-sink", videosink, NULL);

  /* set URI to play back */
  g_object_set (playbin, "uri", uri, NULL);

  /* and GO GO GO! */
  gst_element_set_state (GST_ELEMENT (playbin), GST_STATE_PLAYING);

  /* wait (blocks!) until state change either completes or fails */
  sret = gst_element_get_state (GST_ELEMENT (playbin), NULL, NULL, -1);

  switch (sret) {
    case GST_STATE_CHANGE_FAILURE:{
      msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
      goto got_error_message;
    }
    case GST_STATE_CHANGE_SUCCESS:{
      GstMessage *msg;

      g_print ("Playing ...\n");

      while (1) {
        gint64 dur, pos;

        if (gst_element_query_duration (playbin, GST_FORMAT_TIME, &dur) &&
            gst_element_query_position (playbin, GST_FORMAT_TIME, &pos)) {
          g_print ("  %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\n",
              GST_TIME_ARGS (pos), GST_TIME_ARGS (dur));
        }

        /* check if we finished or if there was an error,
         * but don't wait/block if neither is the case */
        msg = gst_bus_poll (bus, GST_MESSAGE_EOS | GST_MESSAGE_ERROR, 0);

        if (msg && GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR)
          goto got_error_message;

        if (msg && GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS) {
          g_print ("Finished.\n");
          break;
        }

        /* sleep for one second */
        g_usleep (G_USEC_PER_SEC * 1);
      }
      break;
    }
    default:
      g_assert_not_reached ();
  }

  /* shut down and free everything */
  gst_element_set_state (playbin, GST_STATE_NULL);
  gst_object_unref (playbin);
  gst_object_unref (bus);
  return;

/* ERRORS */
got_error_message:
  {
    if (msg) {
      GError *err = NULL;
      gchar *dbg_str = NULL;

      gst_message_parse_error (msg, &err, &dbg_str);
      g_printerr ("FAILED to play %s: %s\n%s\n", uri, err->message,
          (dbg_str) ? dbg_str : "(no debugging information)");
      g_error_free (err);
      g_free (dbg_str);
      gst_message_unref (msg);
    } else {
      g_printerr ("FAILED to play %s: unknown error\n", uri);
    }

    /* shut down and free everything */
    gst_element_set_state (playbin, GST_STATE_NULL);
    gst_object_unref (playbin);
    gst_object_unref (bus);
    return;
  }

no_playbin:
  {
    g_error ("Could not create GStreamer 'playbin' element. "
        "Please install it");
    /* not reached, g_error aborts */
    return;
  }

no_autoaudiosink:
  {
    g_error ("Could not create GStreamer 'autoaudiosink' element. "
        "Please install it");
    /* not reached, g_error aborts */
    return;
  }

no_autovideosink:
  {
    g_error ("Could not create GStreamer 'autovideosink' element. "
        "Please install it");
    /* not reached, g_error aborts */
    return;
  }
}


