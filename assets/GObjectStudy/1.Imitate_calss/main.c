/**
 * 为什么DList类一定要将GObject类作为父类？主要因为GObject类具有以下功能
 * （1）基于引用计数的内存管理
 * （2）对象的析造函数与析构函数
 * （3）可设置对象属性的set/get函数
 * （4）易于使用的信号机制
*/
#include"pm-dlist.h"

int
main(){
  /* GObject 库的类型管理系统的初始化 */
  g_type_init();
  int i;

  PMDList *list;/*类的实例化, 产生对象*/

  /* 进行三次对象实例化 */
  for(i = 0; i < 3; i++){
    /*可以为对象的实例分配内存与初始化，并且将实例的引用计数设为1*/
    list = g_object_new(PM_TYPE_DLIST, NULL);/* 创建对象的一个实例 */
    /* 用于将对象的实例的引用计数减1,并检测对象的实例的引用计数是否为0,若为0,那么便释放对象的实例存储空间 */
    g_object_unref(list);/* 销毁对象的一个实例 */
  }



  return 0;
}