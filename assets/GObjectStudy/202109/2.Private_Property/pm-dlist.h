#ifndef PM_DLIST_H
#define PM_DLIST_H
  
#include <glib-object.h>

/*宏定义 获取PMDList类信息，初始化函数*/
#define PM_TYPE_DLIST (pm_dlist_get_type())
  
typedef struct _PMDList PMDList;
struct  _PMDList {
        GObject parent_instance;
};
  
typedef struct _PMDListClass PMDListClass;
struct _PMDListClass {
        GObjectClass parent_class;
};

typedef struct _PMDListPrivate PMDListPrivate;

#define PM_DLIST_GET_PRIVATE(obj) (\
        G_TYPE_INSTANCE_GET_PRIVATE ((obj), PM_TYPE_DLIST, PMDListPrivate))



GType pm_dlist_get_type (void);
  
#endif