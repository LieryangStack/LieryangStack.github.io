#ifndef KB_BIBTEX_H
#define KB_BIBTEX_H

#include<glib-object.h>

#define MY_TYPE_BIBTEX (my_bibtex_get_type())
#define MY_BIBTEX(object) \
        G_TYPE_CHECK_INSTANCE_CAST ((object), MY_TYPE_BIBTEX, MyBibtex)

typedef struct _MyBibtex MyBibtex;
struct _MyBibtex{
  GObject parent;
};

typedef struct _MyBibtexClass MyBibtexClass;
struct _MyBibtexClass{
  GObjectClass parent_class;
};

GType my_bibtex_get_type(void);

void 
my_bibtex_printf(MyBibtex *self);

#endif
