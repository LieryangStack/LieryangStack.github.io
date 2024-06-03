
#include <glib-object.h>

typedef struct {
  GObject parent_instance;
} TestObject;

typedef struct {
  int dummy_0;
  float dummy_1;
} TestObjectPrivate;

typedef struct {
  GObjectClass parent_class;
} TestObjectClass;

GType test_object_get_type (void);

G_DEFINE_TYPE_WITH_CODE (TestObject, test_object, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (TestObject))



static void
test_object_class_init (TestObjectClass *klass)
{
}

static void
test_object_init (TestObject *self)
{
  TestObjectPrivate *priv = test_object_get_instance_private (self);

  if (g_test_verbose ())
    g_printerr ("Offset of %sPrivate for type '%s': %d\n",
             G_OBJECT_TYPE_NAME (self),
             G_OBJECT_TYPE_NAME (self),
             TestObject_private_offset);

  priv->dummy_0 = 42;
  priv->dummy_1 = 3.14159f;
}

static int
test_object_get_dummy_0 (TestObject *self)
{
  TestObjectPrivate *priv = test_object_get_instance_private (self);

  return priv->dummy_0;
}

static float
test_object_get_dummy_1 (TestObject *self)
{
  TestObjectPrivate *priv = test_object_get_instance_private (self);

  return priv->dummy_1;
}

typedef struct {
  TestObject parent_instance;
} TestDerived;

typedef struct {
  char *dummy_2;
} TestDerivedPrivate;

typedef struct {
  TestObjectClass parent_class;
} TestDerivedClass;

GType test_derived_get_type (void);

G_DEFINE_TYPE_WITH_CODE (TestDerived, test_derived, test_object_get_type (),
                         G_ADD_PRIVATE (TestDerived))

static void
test_derived_finalize (GObject *obj)
{
  TestDerivedPrivate *priv = test_derived_get_instance_private ((TestDerived *) obj);

  g_free (priv->dummy_2);

  G_OBJECT_CLASS (test_derived_parent_class)->finalize (obj);
}

static void
test_derived_class_init (TestDerivedClass *klass)
{
  G_OBJECT_CLASS (klass)->finalize = test_derived_finalize;
}

static void
test_derived_init (TestDerived *self)
{
  TestDerivedPrivate *priv = test_derived_get_instance_private (self);
  priv->dummy_2 = g_strdup ("Hello");
  TestObjectPrivate *parent_priv = test_object_get_instance_private ((TestObject *)self);
  parent_priv->dummy_1 = 1.999;
}

static const char *
test_derived_get_dummy_2 (TestDerived *self)
{
  TestDerivedPrivate *priv = test_derived_get_instance_private (self);

  return priv->dummy_2;
}


int
main (int argc,
      char *argv[]) {
  
  TestObject *object = (TestObject *)g_object_new (test_object_get_type(), NULL);
  TestDerived *derived_object = (TestDerived *)g_object_new (test_derived_get_type(), NULL);

  g_print ("dummy_1 = %0.5f\n", test_object_get_dummy_1((TestObject *)derived_object));
  g_print ("dummy_2 = %s\n", test_derived_get_dummy_2(derived_object));

  g_object_unref (object);
  g_object_unref (derived_object);

  return 0;
}