#include <glib.h>
#include <gio/gio.h>

static gboolean
cockpit_web_request_on_input (GObject *pollable_input,
                              gpointer user_data)
{
  GPollableInputStream *input = (GPollableInputStream *)pollable_input;

  char buffer[255] = {0};

  gint count = g_pollable_input_stream_read_nonblocking (input, buffer, 255, NULL, NULL);

  g_print ("%s n = %d\n", buffer, count);
  
  return TRUE;
}

int
main (int argc, char *argv[]) {

  GError * error = NULL;

  /* create a new connection */
  GSocketConnection * connection = NULL;
  GSocketClient * client = g_socket_client_new();

  GMainContext *context = g_main_context_new ();
  GMainLoop *loop = g_main_loop_new(context, FALSE);

  /* connect to the host */
  connection = g_socket_client_connect_to_host (client,
                                               (gchar*)"192.168.10.126:8500",
                                                1500, /* your port goes here */
                                                NULL,
                                                &error);


  /* don't forget to check for errors */
  if (error != NULL)
  {
      g_error ("error->message");
  }
  else
  {
      g_print ("Connection successful!\n");
  }

  /* use the connection */
  GInputStream * istream = g_io_stream_get_input_stream (G_IO_STREAM (connection));
  GOutputStream * ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));
  GPollableInputStream *poll_in = (GPollableInputStream *)istream;

  if (!poll_in || !g_pollable_input_stream_can_poll (poll_in)) {
    if (istream)
      g_critical ("cannot use a non-pollable input stream: %s", G_OBJECT_TYPE_NAME (istream));
    else
      g_critical ("no input stream available");
    return -1;
  }
  GSource *source = g_pollable_input_stream_create_source (poll_in, NULL);
  g_source_set_callback (source, (GSourceFunc)cockpit_web_request_on_input, NULL, NULL);
  g_source_attach (source, context);

  // g_output_stream_write  (ostream,
  //                         "Hello server!", /* your message goes here */
  //                         13, /* length of your message */
  //                         NULL,
  //                         &error);
  g_main_loop_run (loop);

  /* don't forget to check for errors */
  if (error != NULL)
  {
      g_error ("error->message");
  }
  return 0;
}