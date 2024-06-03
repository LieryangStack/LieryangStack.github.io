
#include <glib-object.h>

typedef struct {
  GObject parent_instance;
} TestObject;

typedef struct {
  GObjectClass parent_class;
} TestObjectClass;

GType test_object_get_type (void);

G_DEFINE_TYPE (TestObject, test_object, G_TYPE_OBJECT)

static void
test_object_finalize (GObject *obj)
{
  g_print ("%s\n", __func__);
  G_OBJECT_CLASS (test_object_parent_class)->finalize (obj);
}

static void
test_object_dispose (GObject *obj)
{
  g_print ("%s\n", __func__);
  G_OBJECT_CLASS (test_object_parent_class)->dispose (obj);
}

static void
test_object_class_init (TestObjectClass *klass) {
  G_OBJECT_CLASS (klass)->finalize = test_object_finalize;
  G_OBJECT_CLASS (klass)->dispose = test_object_dispose;
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
  g_print ("%s\n", __func__);
  G_OBJECT_CLASS (test_derived_parent_class)->finalize (obj);
}

static void
test_derived_dispose (GObject *obj)
{
  g_print ("%s\n", __func__);
  G_OBJECT_CLASS (test_derived_parent_class)->dispose (obj);
}

static void
test_derived_class_init (TestDerivedClass *klass) {
  G_OBJECT_CLASS (klass)->finalize = test_derived_finalize;
  G_OBJECT_CLASS (klass)->dispose = test_derived_dispose;
}

static void
test_derived_init (TestDerived *self)
{
}


int
main (int argc, char *argv[]) {
  
  
  TestObject *object = (TestObject *)g_object_new (test_object_get_type(), NULL);
  
  TestDerived *derived_object = (TestDerived *)g_object_new (test_derived_get_type(), NULL);
  

  g_print ("*************TestObject unref Start****************\n");
  g_object_unref (object);
  g_print ("*************TestObject unref Finish****************\n\n");

  g_print ("*************TestDerived unref Start****************\n");
  g_object_unref (derived_object);
  g_print ("*************TestDerived unref Finish****************\n\n");

  return 0;
}