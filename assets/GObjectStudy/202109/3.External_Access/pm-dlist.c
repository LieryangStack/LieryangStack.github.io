#include"pm-dlist.h"
/**
 * 此宏的三个功能
 * 1.函数GTyep pm_dlist_get_type()的具体实现,由该宏自动实现
 * 2.注册函数前缀pm_dlish到GObject
 * 3.注册实例结构体名称PMDList到GObject
*/
G_DEFINE_TYPE(PMDList, pm_dlist, G_TYPE_OBJECT);

/* 获得私有成员变量结构体地址 */
#define PM_DLIST_GET_PRIVATE(obj) \
        G_TYPE_INSTANCE_GET_PRIVATE ((obj), PM_TYPE_DLIST, PMDListPrivate)

typedef struct _PMDListPrivate PMDListPrivate;
struct  _PMDListPrivate {
        PMDList *head;
        PMDList *tail;
};

/* 属性对应的ID，真正的属性ID不能为0 */
enum PropertyDlist{
  PROPERTY_DLIST_0,/*占位*/
  PROPERTY_DLIST_HEAD,
  PROPERTY_DLIST_TAIL
};

static void
pm_dlist_set_property(GObject *object, guint property_id,
                      const GValue *value, GParamSpec *pspec){
  /*基类转换成子类指针*/
  PMDList *self = PM_DLIST(object);
  /*通过宏获取唯一私有属性地址*/
  PMDListPrivate *priv = PM_DLIST_GET_PRIVATE(self);

  switch (property_id){
    case PROPERTY_DLIST_HEAD:
            priv->head = g_value_get_pointer (value);
            break;
    case PROPERTY_DLIST_TAIL:
            priv->tail = g_value_get_pointer (value);
            break;
    default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
  }
}

static void
pm_dlist_get_property (GObject *object, guint property_id,
                       GValue *value, GParamSpec *pspec)
{
  PMDList *self = PM_DLIST (object);
  PMDListPrivate *priv = PM_DLIST_GET_PRIVATE (self);
  
  switch (property_id) {
  case PROPERTY_DLIST_HEAD:
          g_value_set_pointer (value, priv->head);
          break;
  case PROPERTY_DLIST_TAIL:
          g_value_set_pointer (value, priv->tail);
          break;
  default:
          G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
          break;
  }
}

static void
pm_dlist_class_init(PMDListClass *klass)
{
  /*私有成员添加到类里面*/
  g_type_class_add_private(klass, sizeof(PMDListPrivate));
  
  GObjectClass *base_class = G_OBJECT_CLASS(klass);
  base_class->set_property = pm_dlist_set_property;
  base_class->get_property = pm_dlist_get_property;

  GParamSpec *pspec;
  pspec = g_param_spec_pointer ("head",
                                "Head node",
                                "The head node of the double list",
                                G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (base_class, PROPERTY_DLIST_HEAD, pspec);
  pspec = g_param_spec_pointer ("tail",
                                "Tail node",
                                "The tail node of the double list",
                                G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (base_class, PROPERTY_DLIST_TAIL, pspec);
}

static void
pm_dlist_init (PMDList *self)
{
}