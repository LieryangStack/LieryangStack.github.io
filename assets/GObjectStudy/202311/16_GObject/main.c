
#include <glib-object.h>

typedef struct {
  GObject parent_instance;
} TestObject;

typedef struct {
  GObjectClass parent_class;
  gint a;

} TestObjectClass;

GType test_object_get_type (void);

G_DEFINE_TYPE (TestObject, test_object, G_TYPE_OBJECT)

static void
test_object_class_init (TestObjectClass *klass) {
  g_print ("%s (TestObjectClass = %p)\n", __func__, klass);
  klass->a = 1314;
}

static void
test_object_init (TestObject *self)
{

}


typedef struct {
  TestObject parent_instance;
} TestDerived;

typedef struct {
  TestObjectClass parent_class;
} TestDerivedClass;

GType test_derived_get_type (void);

G_DEFINE_TYPE (TestDerived, test_derived, test_object_get_type())

static void
test_derived_finalize (GObject *obj)
{
  
  G_OBJECT_CLASS (test_derived_parent_class)->finalize (obj);
}

static void
test_derived_class_init (TestDerivedClass *klass) {
  g_print ("test_derived_parent_class->a = %d\n",((TestObjectClass *) test_derived_parent_class)->a);
  g_print ("%s (TestDerivedClass = %p)\n", __func__, klass);
  G_OBJECT_CLASS (klass)->finalize = test_derived_finalize;
}

static void
test_derived_init (TestDerived *self)
{

}


int
main (int argc, char *argv[]) {
  
  TestObject *object = (TestObject *)g_object_new (test_object_get_type(), NULL);
  TestDerived *derived_object = (TestDerived *)g_object_new (test_derived_get_type(), NULL);

  TestObjectClass *object_class = (TestObjectClass *)G_OBJECT_GET_CLASS (object);
  TestDerivedClass *derived_object_class = (TestDerivedClass *)G_OBJECT_GET_CLASS (derived_object);

  g_print ("object_class = %p\n", object_class);
  g_print ("derived_object_class = %p\n", derived_object_class);

  g_object_unref (object);
  g_object_unref (derived_object);

  return 0;
}