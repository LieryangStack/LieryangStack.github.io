#include"my-bibtex.h"

/* 类的数据类型获取函数宏 */
#define MY_TYPE_ARTICLE (my_article_get_type())

#define MY_ARTICLE(object) \
        G_TYPE_CHECK_INSTANCE_CAST ((object), MY_TYPE_ARTICLE, MyArticle)
#define MY_ARTICLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_ARTICLE, MyArticleClass))

typedef struct _MyArticle MyArticle;
struct _MyArticle{
  MyBibtex parent;
};

typedef struct _MyArticleClass MyArticleClass;
struct _MyArticleClass{
  MyBibtexClass parent_class;
  gint test_number;
};

GType
my_article_get_type();

void 
my_article_printf(MyArticle *self);