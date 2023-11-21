/**
  Compile:
      gcc Gvalue.c `pkg-config --cflags --libs glib-2.0 gobject-2.0` 
*/
#include<glib.h>
#include<glib/gprintf.h>
#include<glib-object.h>
#include<stdlib.h>

static void
string2int (const GValue *src_value,
            GValue       *dest_value) {
  const gchar *value = g_value_get_string (src_value);
  int i = atoi(value);
  g_value_set_int (dest_value, i);
}

/* 新建一个GBoxed类型 */
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
G_DEFINE_BOXED_TYPE (MyStruct, my_struct, my_struct_copy, my_struct_free);


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
}

MyStruct *    
my_struct_copy (MyStruct *self){

} 

static inline void		/* keep this function in sync with gvaluecollector.h and gboxed.c */
value_meminit (GValue *value,
	       GType   value_type)
{
  value->g_type = value_type;
  memset (value->data, 0, sizeof (value->data));
}


/********************GObject************************/
#define T_TYPE_DOUBLE  (t_double_get_type ())

typedef struct _TDouble TDouble;
struct _TDouble {
  GObject parent;
  double value;
};

typedef struct _TDoubleClass TDoubleClass;
struct _TDoubleClass {
  GObjectClass parent_class;
};

G_DEFINE_TYPE (TDouble, t_double, G_TYPE_OBJECT)

static void
t_double_class_init (TDoubleClass *class) {
}

static void
t_double_init (TDouble *self) {
  
}


void 
test () {
  GValue a = G_VALUE_INIT;
  GValue b = G_VALUE_INIT;
  value_meminit (&a, T_TYPE_DOUBLE);
  value_meminit (&b, G_TYPE_OBJECT);
  
  GTypeValueTable *value_table_1 = g_type_value_table_peek (T_TYPE_DOUBLE);
  GTypeValueTable *value_table_2 = g_type_value_table_peek (G_TYPE_OBJECT);
  
  /* 经测试，使用的同一个 value_init */
  g_print ("value_table_1->value_init = %p\n", value_table_1->value_init);
  g_print ("value_table_2->value_init = %p\n", value_table_2->value_init);
}

int 
main (int argc, char *argv[]) {
  /*Gvalues 一定要被初始化（因为内部用的是联合体，所谓初始化就是内部空间全赋值为0）*/
  GValue a = G_VALUE_INIT;
  GValue b = G_VALUE_INIT;

  // test();
  
  /* g_assert判断是否是G_VALUE_HOLDS_STRING字符串类型
   * 因为现在和没有初始化类型，G_VALUE_HOLDS_STRING(&a) 等于 0
   */
  g_assert(!G_VALUE_HOLDS_STRING(&a));

  /*把变量a初始化为字符串类型*/
  g_value_init(&a, G_TYPE_STRING);
  g_assert(G_VALUE_HOLDS_STRING(&a));
  g_value_set_string(&a, "Hello World");
  g_printf("%s\n", g_value_get_string(&a));
  
  /* 必须重置变量a恢复到原始状态 */
  g_value_unset(&a);

  /* 必须初始化成uint64，否则报错 */
  g_value_init(&a, G_TYPE_INT);
  g_value_set_uint64(&a, 12);
  g_printf("%ld\n", g_value_get_uint64(&a));

  g_value_init(&b, G_TYPE_STRING);
  g_assert(g_value_type_transformable(G_TYPE_INT, G_TYPE_STRING));

  g_value_transform(&a, &b);
  g_printf("%s\n", g_value_get_string(&b));

  /* An STRINT is not transformable to INT */
  if (g_value_type_transformable (G_TYPE_STRING, G_TYPE_INT)) {
      g_printf ("Can transform string to int\n");
  } else
      g_printf ("Can Not transform string to int\n");
  

  /* Attempt to transform it again using a custom transform function */
  g_value_set_string(&b, "43");
  g_value_register_transform_func (G_TYPE_STRING, G_TYPE_INT, string2int);
  g_value_transform (&b, &a);
  g_printf ("%d\n", g_value_get_int(&a));

  return 0;
}
