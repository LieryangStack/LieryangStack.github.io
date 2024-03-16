#include"my-article.h"
#include"my-bibtex.h"

G_DEFINE_TYPE(MyArticle, my_article, MY_TYPE_BIBTEX);
#define MY_ARTICLE_GET_PRIVATE(object) (\
        G_TYPE_INSTANCE_GET_PRIVATE ((object), MY_TYPE_ARTICLE, MyArticlePrivate))


/* 对象私有属性 */
typedef struct _MyArticlePrivate MyArticlePrivate;
struct _MyArticlePrivate{
  GString *journal;
  GString *volume;
  GString *number;
  GString *pages;
};

enum PROPERTY_ARTICLE {
  PROPERTY_0,
  PROPERTY_JOURNAL,
  PROPERTY_VOLUME,
  PROPERTY_NUMBER,
  PROPERTY_PAGES,
  N_PROPERTIES
};

static void
my_article_set_property (GObject *object, guint property_id,
                        const GValue *value, GParamSpec *pspec)
{       
  MyArticle *self = MY_ARTICLE (object);
  MyArticlePrivate *priv = MY_ARTICLE_GET_PRIVATE (self);
    
  switch (property_id) {
  case PROPERTY_JOURNAL:
        if (priv->journal)
                g_string_free (priv->journal, TRUE);
        priv->journal = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_VOLUME:
        if (priv->volume)
                g_string_free (priv->volume, TRUE);
        priv->volume = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_NUMBER:
        if (priv->number)
                g_string_free (priv->number, TRUE);
        priv->number = g_string_new (g_value_get_string (value));
        break;
  case PROPERTY_PAGES:
        if (priv->pages)
                g_string_free (priv->pages, TRUE);
        priv->pages = g_string_new (g_value_get_string (value));
        break;
  default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
  }
}
 
static void
my_article_get_property (GObject *object, guint property_id,
                        GValue *value, GParamSpec *pspec)
{
  MyArticle *self = MY_ARTICLE (object);
  MyArticlePrivate *priv = MY_ARTICLE_GET_PRIVATE (self);
  GString *similar = NULL;
    
  switch (property_id) {
  case PROPERTY_JOURNAL:
        g_value_set_string (value, priv->journal->str);
        break;
  case PROPERTY_VOLUME:
        g_value_set_string (value, priv->volume->str);
        break;
  case PROPERTY_NUMBER:
        g_value_set_string (value, priv->number->str);
        break;
  case PROPERTY_PAGES:
        g_value_set_string (value, priv->pages->str);
        break;
  default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
  }
}

/* 参数名之所以使用klass而不是class，为了C++编译的时候防止和关键字class冲突 */
static void
my_article_class_init (MyArticleClass *klass)
{
  g_type_class_add_private (klass, 
                            sizeof (MyArticlePrivate));
  klass->test_number = 10;
  GObjectClass *base_class = G_OBJECT_CLASS (klass);
  g_printf("Article***: %p\n", base_class);
  base_class->set_property = my_article_set_property;
  base_class->get_property = my_article_get_property;
  GParamSpec *properties[N_PROPERTIES] = {NULL,};
  properties[PROPERTY_JOURNAL] =
          g_param_spec_string ("journal",
                                "Journal",
                                "Bibliography journal",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_VOLUME] =
          g_param_spec_string ("volume",
                                "Volume",
                                "Bibliography volume",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_NUMBER] =
          g_param_spec_string ("number",
                                "Number",
                                "Bibliography number",
                                NULL,
                                G_PARAM_READWRITE);
  properties[PROPERTY_PAGES] =
          g_param_spec_string ("pages",
                              "Pages",
                              "Bibliography pages",
                              NULL,
                              G_PARAM_READWRITE);
  g_object_class_install_properties (base_class, N_PROPERTIES, properties);
}

static void
my_article_init (MyArticle *self)
{
}


void 
my_article_printf(MyArticle *self)
{
  my_bibtex_printf(&self->parent);

  gchar *journal, *volume, *number, *pages;

  g_object_get(G_OBJECT(self),
               "journal", &journal,
               "volume", &volume,
               "number", &number,
               "pages", &pages,
               NULL);

  g_printf("  Journal: %s\n"
           "   Volume: %s\n"
           "   Number: %s\n"
           "    Pages: %s\n", journal, volume, number, pages);
  
  /*g_free 是因为系统重新创建了字符串内存空间，没有指向私有属性的地址*/
  g_free(journal);
  g_free(volume);
  g_free(number);
  g_free(pages);
}
