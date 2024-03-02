#include <glib-object.h>

#define MY_TYPE_STRUCT my_struct_get_type()
GType my_struct_get_type (void) G_GNUC_CONST;

typedef struct _MyStruct MyStruct;

struct _MyStruct {
  char *name;
  int id;
};

MyStruct *    my_struct_new (void);
void          my_struct_free (MyStruct *self);
MyStruct *    my_struct_copy (MyStruct *self);

/**
 * my_struct_copy 通过 g_boxed_copy 可以调用
 * my_struct_free 调用 g_boxed_free 可以调用
*/
G_DEFINE_BOXED_TYPE_WITH_CODE (MyStruct, my_struct, my_struct_copy, my_struct_free, g_print("hello\n"));

MyStruct *    
my_struct_new (void) {
  MyStruct *mystrcut = g_new (MyStruct, 1);
  mystrcut->name = g_malloc0 (50);
  return mystrcut;
}

MyStruct *    
my_struct_new_with_name (char *name) {
  MyStruct *mystrcut = g_new (MyStruct, 1);
  mystrcut->name = g_malloc0 (50);
  memcpy (mystrcut->name, name, sizeof(name));
  return mystrcut;
}

void          
my_struct_free (MyStruct *self){
  g_free (self->name);
  g_free (self);
  g_print ("%s\n", __func__);
}

MyStruct *    
my_struct_copy (MyStruct *self){

} 

int
main (int argc, char* argv[]) {

  g_print ("%s\n", __func__);

  // g_object_new (MY_TYPE_STRUCT, NULL);

  MyStruct *myS1 = my_struct_new_with_name ("Xiaoli\n");

  g_print ("%s", myS1->name);

  /* 没有引用计数，需要调用free释放 */
  my_struct_free (myS1);

  // g_object_unref (myS1);

  return 0;
}