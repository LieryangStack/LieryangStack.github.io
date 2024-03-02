#include <gst/gst.h>
#include <gst/gststructure.h>

int 
main (int argc, char *argv[]) {

  gst_init(&argc, &argv);

  const char *strings[] = {
    "video/x-raw, width = (int) 123456",
    "video/x-raw, stride = (int) -123456",
    "video/x-raw, red_mask = (int) 0xFFFF",
    "video/x-raw, red_mask = (int) 0x0000FFFF",
    "video/x-raw, red_mask = (int) 0x7FFFFFFF",
    "video/x-raw, red_mask = (int) 0x80000000",
    "video/x-raw, red_mask = (int) 0xFF000000",
    /* result from
     * gst-launch ... ! "video/x-raw, red_mask=(int)0xFF000000" ! ... */
    "video/x-raw,\\ red_mask=(int)0xFF000000",
  };

  GstStructure *structure = NULL;
  int i;

  for (i = 0; i < G_N_ELEMENTS (strings); ++i) {
    const char *s;
    const gchar *name;
    gint value;
    gchar *end;

    s = strings[i];
 
    structure = gst_structure_from_string (s, &end);
    g_print ("end = %p  s = %p\n", end, s + strlen(s));
    g_print ("gst_structure_get_name(structure) = %s\n", gst_structure_get_name(structure));
    if (structure == NULL)
      g_print ("Could not get structure from string %s\n", s);
    name = gst_structure_nth_field_name (structure, 0);
    if (name)
      g_print ("i = %d name = %s\n", i, name);

    /* cleanup */
    gst_structure_free (structure);
  }

  return 0;
}