#include <glib-object.h>

int
main (int argc, char **argv) {
  GObject *instance1, *instance2, *instance3;
  GObjectClass *class1, *class2, *class3;
  
  g_print ("%ld\n", G_TYPE_OBJECT);

  instance1 = g_object_new (G_TYPE_OBJECT, NULL);
  instance2 = g_object_new (G_TYPE_OBJECT, NULL);
  g_print ("The address of instance1 is %p\n", instance1);
  g_print ("The address of instance2 is %p\n", instance2);

  class1 = G_OBJECT_GET_CLASS (instance1);
  class2 = G_OBJECT_GET_CLASS (instance2);
  g_print ("The address of the class of instance1 is %p\n", class1);
  g_print ("The address of the class of instance2 is %p\n", class2);

  g_object_unref (instance1);
  g_object_unref (instance2);

  /* 销毁GObject实例。实例的内存被释放。GObject类型是静态类型。
   （注意静态类型不会销毁类，动态类型在销毁最后一个实例时销毁其类）
     静态类型从不破坏它的类。因此，即使被销毁的实例是最后一个实例，类仍然存在。 
     
     所以，打印class3的类地址和class2，class1相同 
     */
  instance3 = g_object_new (G_TYPE_OBJECT, NULL);
  class3 = G_OBJECT_GET_CLASS (instance3);
  g_print ("The address of the class of instance3 is %p\n", class3);
  g_object_unref (instance3);

  return 0;
}

