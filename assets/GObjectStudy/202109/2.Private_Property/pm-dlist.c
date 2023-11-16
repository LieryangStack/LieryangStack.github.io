#include "pm-dlist.h"

/*生成pm_dlist_get_type函数*/
G_DEFINE_TYPE (PMDList, pm_dlist, G_TYPE_OBJECT);
 
typedef struct _PMDListNode PMDListNode;
struct  _PMDListNode {
        PMDListNode *prev;
        PMDListNode *next;
        void *data;
};

struct  _PMDListPrivate {
        PMDListNode *head;
        PMDListNode *tail;
};
 
static void
pm_dlist_class_init (PMDListClass *klass)
{
        g_type_class_add_private (klass, sizeof (PMDListPrivate));
}
 
static void
pm_dlist_init (PMDList *self)
{
        PMDListPrivate *priv = PM_DLIST_GET_PRIVATE (self);
         
        priv->head = 0x3333;
        priv->tail = 0x2222;
}