#include"my-bibtex.h"
#include"my-article.h"

int
main (void)
{
  g_type_init ();
    
  MyArticle *entry = g_object_new (MY_TYPE_ARTICLE,
                                    "title", "Breaking paragraphs into lines",
                                    "author", "Knuth, D.E. and Plass, M.F.",
                                    "publisher", "Wiley Online Library",
                                    "journal", "Software: Practice and Experience",
                                    "volume", "11",
                                    "number", "11",
                                    "year", 1981,
                                    "pages", "1119-1184",
                                    NULL);
  MyArticle *t = g_object_new(MY_TYPE_ARTICLE,NULL);

  g_printf("%d\n",MY_ARTICLE_GET_CLASS(t)->test_number);
  
  my_article_printf (entry);

  g_object_unref (entry);
  return 0;
}
