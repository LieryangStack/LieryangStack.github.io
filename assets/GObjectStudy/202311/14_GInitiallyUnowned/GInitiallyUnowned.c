#include <glib-object.h>

int 
main (int argc, char *argv[]){

  GObject *object = g_object_new (G_TYPE_INITIALLY_UNOWNED, NULL);
  /* 处于浮点引用状态直接，引用一次 */
  g_object_ref (object);
  /* 此时 object->ref_count = 2 
   * 也就说明：GInitiallyUnowned浮点引用状态的时候可以被直接 ref
   */
  g_print ("object->ref_countd = %d\n", object->ref_count);

  /**
   * object->qdata = 0x2
   * 标记该对象为浮点引用，实际操作的是GObject结构体成员qdata内容
   * 由qdata数据的第2个bit作为Flag记录浮点引用状态
  */
  g_print ("object->qdata = %p\n", object->qdata);

  /**
   * 设置 GInitiallyUnowned 对象的 qdata
   * g_strdup (Lieryang) = 0x558337916c90
  */
  gchar *name = g_strdup ("Lieryang");
  g_print ("g_strdup (Lieryang) = %p\n", name);

  /**
   * 设置qdata，内部函数是创建一个内存空间给 object->qdata
   * object->qdata = 0x558337916da2
  */
  g_object_set_qdata (object, g_quark_from_string(name), name);
  g_print ("object->qdata = %p\n", object->qdata);

  /*  浮点引用变成正常引用 */
  g_object_ref_sink (object);

  /**
   * g_object_is_floating = 0，说明变成了正常引用
   * object->ref_countd = 2，说明浮点引用变成正常引用，不会改变引用数字（如果是正常引用调用 g_object_ref_sink ，引用数 +1）
   * object->qdata = 0x558337916da0，说明修改第二个bit为零了  0x558337916da2 & ~0x2 =  0x558337916da0
  */
  g_print ("g_object_is_floating = %d\n", g_object_is_floating(object));
  g_print ("object->ref_countd = %d\n", object->ref_count);
  g_print ("object->qdata = %p\n", object->qdata);

  /**
   * get_qdata = 0x558337916c90 get_qdata = Lieryang
   * 能够正常获取qdata，说明该成员没有直接存储qdata数据，中间肯定经过了处理。
  */
  gchar *qdata = g_object_get_qdata (object, g_quark_from_string(name));
  g_print ("get_qdata = %p get_qdata = %s\n", qdata, qdata);

  /**
   * object->ref_countd = 3
   * 正常引用调用 g_object_ref_sink ，引用数 +1
  */
  g_object_ref_sink (object);
  g_print ("object->ref_countd = %d\n", object->ref_count);


  while (object->ref_count > 1)
    g_object_unref (object);
  g_object_unref (object);

  return 0;
}