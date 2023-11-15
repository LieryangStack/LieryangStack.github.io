/**
 * 由于c语言没有为数据类型提供自定义命名空间的功能，
 * 程序中所有的数据类型（包括函数）均处于同一命名，
 * 这样数据类型便存在因为同名而撞车的可能性。
 * 我们可以为这种命名方式取一个名字，叫做PT格式
 * P是项目名称缩写，T是数据类型的名称。
 * 对于一个多面体建模（Polyhedron Modeling）
 * 我们要为这个项目定义一个双向链表的数据类型，
 * 通常是将dlish.h文件名修改为pm-dlish.h
*/

#ifndef PM_DLIST_H
#define PM_DLIST_H

#include<glib-object.h>

/* pm_dlist_get_type 函数的作用就是获取PMDList类的具体结构 */
#define PM_TYPE_DLIST (pm_dlist_get_type())

typedef struct _PMDListNode PMDListNode;
struct _PMDListNode{
/* private */
  PMDListNode *prev;
  PMDListNode *next;
/* public */
  void *data;
};

/**
 * GObject中，类是两个结构体的组合
 * 一个是实例结构体
 * 另一个是类结构提
*/

/**
 * GObject结构体作为PMDList结构体第一个成员
 * 这就意味着PMDList类继承自GObjext类
 */

/* 实例结构体 */
typedef struct _PMDList PMDList;
struct _PMDList{
  GObject parent_instance;
  PMDListNode *head;
  PMDListNode *tail;
};


/* 类结构体 */
typedef struct _PMDListClass PMDListClass;
struct _PMDListClass{
  GObjectClass parent_class;
};

GType pm_dlist_get_type(void);


#endif