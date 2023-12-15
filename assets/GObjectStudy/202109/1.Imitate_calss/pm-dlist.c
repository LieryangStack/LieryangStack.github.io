#include "pm-dlist.h"

/* G_DEFINE_TYPE 宏可以为我们生成pm_dlist_get_type函数的实现代码 */
/**
 * 该宏接受三个参数
 * Param 1: 类名
 * Param 2: 类的成员函数的前缀，例如pm_dlist_get_type中 ”pm_dlist“是它的前缀
 * Param 3: 指明PMDList的父类为G_TYPE_OBJECT
 * G_TYPE_OBJECT指代g_object_get_type函数
*/
G_DEFINE_TYPE(PMDList, pm_dlist, G_TYPE_OBJECT);

/**
 * pm_dlist是前缀,对应
 * pm_dlist_get_type 中保含一下两个初始化函数
 * pm_dlist_init
 * pm_dlist_class_init
*/

/*所有私有对象的数据，则保存在实例结构体中*/
static
void pm_dlist_init(PMDList *self){
  g_printf("\t实例结构体初始化!\n");
  self->head = NULL;
  self->tail = NULL;
  
}


/*所有实例共享的数据，可保存在类结构体*/
static
void pm_dlist_class_init(PMDListClass *klass){
  g_printf("类结构体初始化！\n");
}

