#include "my-file.h"
 
static void
file_print (gpointer gobject, gpointer user_data)
{       
  g_printf ("invoking file_print!\n");
}
 
static void
file_print_xml (gpointer gobject, gpointer user_data)
{       
  g_printf ("invoking file_print_xml!\n");
}
 
static void
file_print_tex (gpointer gobject, gpointer user_data)
{       
  g_printf ("invoking file_print_tex!\n");
}
 
int
main (void)
{
  g_type_init ();

  MyFile *file = g_object_new (MY_TYPE_FILE, "name", "test.txt", NULL);
    
  g_signal_connect (file, "file_changed", G_CALLBACK (file_print), NULL);
  g_signal_connect (file, "file_changed", G_CALLBACK (file_print_xml), NULL);
  g_signal_connect (file, "file_changed", G_CALLBACK (file_print_tex), NULL);
    
  my_file_write (file, "hello world!\n");
    
  g_object_unref (file);
            
  return 0;
}