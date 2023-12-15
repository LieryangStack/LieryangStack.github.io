/*************************************************************************************************************************************************
 * @filename: gstmemory_alloc.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-27-2023
 * @brief: 证明存写锁的时候，还能上写锁。
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

int 
main (int argc, char *argv[]) {

  GstMemory *memory;
  GstMapInfo info;
  int ret;

  gst_init(&argc, &argv);

  /* 使用默认的内存分配器分配内存,不使用分配器变量 */
  memory = gst_allocator_alloc (NULL, 4, NULL);
  
  /* 把内存信息存储到GstMapInfo结构体中 */
  ret = gst_memory_map (memory, &info, GST_LOCK_FLAG_WRITE | GST_LOCK_FLAG_EXCLUSIVE);
  g_print ("ret = %d\n", ret);
  ret = gst_memory_map (memory, &info, GST_LOCK_FLAG_WRITE | GST_LOCK_FLAG_EXCLUSIVE);
  g_print ("ret = %d\n", ret); /* 上锁失败 */
  ret = gst_memory_map (memory, &info, GST_LOCK_FLAG_WRITE | GST_LOCK_FLAG_EXCLUSIVE);
  g_print ("ret = %d\n", ret); /* 上锁失败 */

  ret = gst_memory_map (memory, &info, GST_MAP_WRITE);
  g_print ("ret = %d\n", ret); /* 上锁成功 */
  ret = gst_memory_map (memory, &info, GST_MAP_WRITE);
  g_print ("ret = %d\n", ret); /* 上锁成功 */

  g_print ("GST_MINI_OBJECT(memory)->lockstate = %X\n",GST_MINI_OBJECT(memory)->lockstate);

  gst_memory_unmap (memory, &info);

  gst_memory_unref (memory);

  return 0;
}