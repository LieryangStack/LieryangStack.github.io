/*************************************************************************************************************************************************
 * @filename: gst_structure_new_valist.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-22-2023
 * @brief: 学习 gst_structure_new_valist 的使用
 * @note: 
 * @history: 
 *          1. Date:
 *             Author:
 *             Modification:
 *      
 *          2. ..
 *************************************************************************************************************************************************
*/
#include <gst/gst.h>
#include <gst/gststructure.h>

/**
 * *************************************************************************************************************************************************
 * @brief: 创建GstStructure结构体，并赋值键值对，最后打印输出结构体键值对内容
 * @param name: GstStructure结构体名称
 * @param first: 可变参数列表
 * @return:
 * @note:
 * *************************************************************************************************************************************************
*/
static void
log_gst_structure (const gchar * name, 
                   const gchar * first, ...) {
  va_list var_args;
  GstStructure *s;
  gchar *l;

  va_start (var_args, first);
  s = gst_structure_new_valist (name, first, var_args);

  /* 序列化打印字符串 */
  l = gst_structure_to_string (s);
  g_print ("structure name = %s\n\n", l);
  g_free (l);
  gst_structure_free (s);
  va_end (var_args);
}


int 
main (int argc, char *argv[]) {

  gst_init(&argc, &argv);

  log_gst_structure ("name",
        "ts", G_TYPE_UINT64, (guint64) 0,
        "index", G_TYPE_UINT, 10,
        "test", G_TYPE_STRING, "hallo",
        "bool", G_TYPE_BOOLEAN, TRUE,
        "flag", GST_TYPE_PAD_DIRECTION, GST_PAD_SRC, NULL);

  return 0;
}