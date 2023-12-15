/*************************************************************************************************************************************************
 * @filename: ximagepool.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-20-2023
 * @brief: 如果使用 g_quark_from_static_string 创建动态字符串的 GQuark，动态字符串修改内容后，GQuark无法获取
 * @note: 
 * @history: 
 *          1. Date:
 *             Author:
 *             Modification:
 *      
 *          2. ..
 *************************************************************************************************************************************************
*/

#include <glib.h>

int
main(int argc, char *argv[]){

  GQuark static_quark = 0;
  GQuark quark = 0;
  /* 常量字符串 */
  const gchar *static_name = "EryangLi";
  /* 堆区创建的字符串，该字符串可能会修改 */
  gchar *name = g_strdup (static_name);

  quark = g_quark_from_static_string(name);

  g_print("%s has been Quark %d\n",name, quark);

  name[0] = '@';
  /* 修改后无法找到对应的Quark */
  g_print ("g_quark_try_string(EryangLi) = %d\n", g_quark_try_string(name));
  /* 通过 quark = g_quark_from_static_string(name); 找到的字符串已经被修改了 */
  g_print("%s pointer %p\n%s Quark pointer %p\n", name, name, name, g_quark_to_string(quark));
  
  return 0;
}
