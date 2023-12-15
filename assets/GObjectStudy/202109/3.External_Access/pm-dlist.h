#ifndef PM_DLIST_H
#define PM_DLIST_H

#include <glib-object.h>

#define PM_TYPE_DLIST (pm_dlist_get_type ())
/* 将基类指针类型转换为PMDList类的指针类型 */
#define PM_DLIST(object) G_TYPE_CHECK_INSTANCE_CAST ((object),\
                                                      PM_TYPE_DLIST, PMDList)

typedef struct _PMDList PMDList;
struct  _PMDList {
        GObject parent_instance;
};

typedef struct _PMDListClass PMDListClass;
struct _PMDListClass {
        GObjectClass parent_class;
};

GType pm_dlist_get_type (void);

#endif