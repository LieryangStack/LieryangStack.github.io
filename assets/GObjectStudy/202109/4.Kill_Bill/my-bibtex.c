#include"my-bibtex.h"

/* 类型注册 系统自动生成my_bibtex_get_type()函数 */
G_DEFINE_TYPE (MyBibtex, my_bibtex, G_TYPE_OBJECT);

#define MY_BIBTEX_GET_PRIVATE(object) (\
        G_TYPE_INSTANCE_GET_PRIVATE ((object), MY_TYPE_BIBTEX, MyBibtexPrivate))

/* 对象私有属性 */
typedef struct _MyBibtexPrivate MyBibtexPrivate;
struct _MyBibtexPrivate{
  GString *title;
  GString *author;
  GString *publisher;
  guint year;
};

enum PROPERTY_BIBTEX {
  PROPERTY_0,
  PROPERTY_TITLE,
  PROPERTY_AUTHOR,
  PROPERTY_PUBLISHER,
  PROPERTY_YEAR,
  N_PROPERTIES
};

static void
my_bibtex_set_property (GObject *object, guint property_id,
                        const GValue *value, GParamSpec *pspec)
{       
  MyBibtex *self = MY_BIBTEX (object);
  MyBibtexPrivate *priv = MY_BIBTEX_GET_PRIVATE (self);
    
  switch (property_id) {
  case PROPERTY_TITLE:
        if (priv->title)
                g_string_free (priv->title, TRUE);
        priv->title = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_AUTHOR:
        if (priv->author)
                g_string_free (priv->author, TRUE);
        priv->author = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_PUBLISHER:
        if (priv->publisher)
                g_string_free (priv->publisher, TRUE);
        priv->publisher = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_YEAR:
        priv->year = g_value_get_uint (value);
        break;
  default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
  }
}
 
static void
my_bibtex_get_property (GObject *object, guint property_id,
                        GValue *value, GParamSpec *pspec)
{
  MyBibtex *self = MY_BIBTEX (object);
  MyBibtexPrivate *priv = MY_BIBTEX_GET_PRIVATE (self);
  GString *similar = NULL;
    
  switch (property_id) {
  case PROPERTY_TITLE:
        g_value_set_string (value, priv->title->str);
        break;
  case PROPERTY_AUTHOR:
        g_value_set_string (value, priv->author->str);
        break;
  case PROPERTY_PUBLISHER:
        g_value_set_string (value, priv->publisher->str);
        break;
  case PROPERTY_YEAR:
        g_value_set_uint (value, priv->year);
        break;
  default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
  }
}

/* 参数名之所以使用klass而不是class，为了C++编译的时候防止和关键字class冲突 */
static void
my_bibtex_class_init (MyBibtexClass *klass)
{
  g_type_class_add_private (klass, 
                            sizeof (MyBibtexPrivate));
  GObjectClass *base_class = G_OBJECT_CLASS (klass);
  base_class->set_property = my_bibtex_set_property;
  base_class->get_property = my_bibtex_get_property;
  GParamSpec *properties[N_PROPERTIES] = {NULL,};
  properties[PROPERTY_TITLE] =
          g_param_spec_string ("title",
                                "Title",
                                "Bibliography title",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_AUTHOR] =
          g_param_spec_string ("author",
                                "Author",
                                "Bibliography author",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_PUBLISHER] =
          g_param_spec_string ("publisher",
                                "Publisher",
                                "Bibliography Publisher",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_YEAR] =
          g_param_spec_uint ("year",
                              "Year",
                              "Bibliography year",
                              0,
                              G_MAXUINT,
                              0,
                              G_PARAM_READWRITE);
  g_object_class_install_properties (base_class, N_PROPERTIES, properties);
}

static void
my_bibtex_init (MyBibtex *self)
{
}

void 
my_bibtex_printf(MyBibtex *self)
{
  gchar *title, *author, *publisher;
  gint year;

  g_object_get(G_OBJECT(self),
               "title", &title,
               "author", &author,
               "publisher", &publisher,
               "year", &year,
               NULL);

  g_printf("    Title: %s\n"
           "   Author: %s\n"
           "Publisher: %s\n"
           "     Year: %d\n", title, author, publisher, year);
  
  /*g_free 是因为系统重新创建了字符串内存空间，没有指向私有属性的地址*/
  g_free(title);
  g_free(author);
  g_free(publisher);
}