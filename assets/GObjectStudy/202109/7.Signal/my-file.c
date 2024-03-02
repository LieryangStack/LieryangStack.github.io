#include "my-file.h"

G_DEFINE_TYPE(MyFile, my_file, G_TYPE_OBJECT);

#define MY_FILE_GET_PRIVATE(object) (\
                G_TYPE_INSTANCE_GET_PRIVATE ((object), MY_TYPE_FILE, MyFilePrivate))

typedef struct  _MyFilePrivate MyFilePrivate;
struct _MyFilePrivate
{
  GString *name;
  GIOChannel *file;
};

enum PropertyDList {
  PROPERTY_FILE_0,
  PROPERTY_FILE_NAME
};

static void
my_file_dispose(GObject *gobject)
{
  MyFile *self = MY_FILE(gobject);
  MyFilePrivate *priv = MY_FILE_GET_PRIVATE(self);
  if(priv->file){
    g_io_channel_unref(priv->file);
    priv->file = NULL;
  }
  G_OBJECT_CLASS(my_file_parent_class)->dispose(gobject);
}

static void
my_file_finalize(GObject *gobject)
{
  MyFile *self = MY_FILE(gobject);
  MyFilePrivate *priv = MY_FILE_GET_PRIVATE(self);
  g_string_free(priv->name, TRUE);

  G_OBJECT_CLASS(my_file_parent_class)->finalize(gobject);
}

static void
my_file_set_property (GObject *object, guint property_id,
                      const GValue *value, GParamSpec *pspec)
{
  MyFile *self = MY_FILE (object);
  MyFilePrivate *priv = MY_FILE_GET_PRIVATE (self);

  switch (property_id){
  case PROPERTY_FILE_NAME:
    if (priv->name)
      g_string_free (priv->name, TRUE);
    priv->name = g_string_new (g_value_get_string (value));
    priv->file = g_io_channel_new_file (priv->name->str, "a+", NULL);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}
 
static void
my_file_get_property (GObject *object, guint property_id,
                      GValue *value, GParamSpec *pspec)
{
  MyFile *self = MY_FILE (object);
  MyFilePrivate *priv = MY_FILE_GET_PRIVATE (self);

  switch (property_id){
  case PROPERTY_FILE_NAME:
    g_value_set_string (value, priv->name->str);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static
void my_file_init(MyFile *self)
{
}

static
void my_file_class_init (MyFileClass *klass)
{
  g_type_class_add_private (klass, sizeof (MyFilePrivate));
    
  GObjectClass *base_class = G_OBJECT_CLASS (klass);
  base_class->set_property = my_file_set_property;
  base_class->get_property = my_file_get_property;
  base_class->dispose      = my_file_dispose;
  base_class->finalize     = my_file_finalize;

  GParamSpec *pspec;
  pspec = g_param_spec_string ("name",
                                "Name",
                                "File name",
                                NULL,
                                G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (base_class, PROPERTY_FILE_NAME, pspec);
    
  g_signal_new ("file_changed",
                MY_TYPE_FILE,
                G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                0,
                NULL,
                NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE,
                0);
}

void
my_file_write (MyFile *self, gchar *buffer)
{
  MyFilePrivate *priv = MY_FILE_GET_PRIVATE (self);
  g_io_channel_write_chars (priv->file, buffer, -1, NULL, NULL);
  g_io_channel_flush (priv->file, NULL);
    
  g_signal_emit_by_name(self, "file_changed");  
}


