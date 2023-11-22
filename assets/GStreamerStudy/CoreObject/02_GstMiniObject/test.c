#include <glib-object.h>
#include <gst/gst.h>

int 
main (){

  gst_init (NULL, NULL);

  GObject *object = g_object_new (GST_TYPE_OBJECT, NULL);

  g_print ("object->qdata = %p\n", object->qdata);

  gchar *name = g_strdup ("Lieryang");

  g_object_set_data (object, "name", name);

  g_print ("object->qdata = %p\n", object->qdata);

  g_object_ref_sink (object);

  g_print ("g_object_is_floating = %d\n", g_object_is_floating(object));

  g_print ("object->qdata = %p\n", object->qdata);

  gchar *qdata = g_object_get_qdata (object, g_quark_from_static_string(name));
  g_print ("qdata = %p qdata = %s\n", qdata, qdata);

  g_object_unref (object);

  return 0;
}