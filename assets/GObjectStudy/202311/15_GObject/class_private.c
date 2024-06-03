#include <glib-object.h>

typedef struct _MyObject MyObject;
typedef struct _MyObjectClass MyObjectClass;
typedef struct _MyObjectClassPrivate MyObjectClassPrivate;

struct _MyObject
{
  GObject parent_instance;

  gint count;
};

struct _MyObjectClass
{
  GObjectClass parent_class;
};

struct _MyObjectClassPrivate
{
  gint secret_class_count;
};

static GType my_object_get_type (void);
G_DEFINE_TYPE_WITH_CODE (MyObject, my_object, G_TYPE_OBJECT,
                         g_type_add_class_private (g_define_type_id, sizeof (MyObjectClassPrivate)) );

static void
my_object_init (MyObject *obj)
{
  obj->count = 42;
}

static void
my_object_class_init (MyObjectClass *klass)
{
}

int
main(void)
{
  GObject *obj;
  MyObjectClass *class;
  MyObjectClassPrivate *priv;

  obj = g_object_new (my_object_get_type (), NULL);

  class = g_type_class_ref (my_object_get_type ());
  priv = G_TYPE_CLASS_GET_PRIVATE (class, my_object_get_type (), MyObjectClassPrivate);
  priv->secret_class_count = 13;
  g_type_class_unref (class);

  g_object_unref (obj);

  return 0;
}