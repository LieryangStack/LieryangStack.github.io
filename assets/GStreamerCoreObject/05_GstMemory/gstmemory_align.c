/*************************************************************************************************************************************************
 * @filename: gstmemory_alloc.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-27-2023
 * @brief: 学习 GstMemory 的基本使用
 *         1. 使用默认分配器分配GstMemory（不使用GstAllocationParams）
 *         2. 使用默认分配器分配GstMemory（使用GstAllocationParams）
 *         3. 使用映射函数，获取data地址
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
  
  gst_memory_map (memory, &info, GST_MAP_WRITE);

  /* 用户实际请求的size，也就是分配器设定的size，info.size = 4 */
  g_print ("info.size = %ld\n",info.size);
  /* info.maxsize = 7 */
  g_print ("info.maxsize = %ld\n",info.maxsize);

  g_print ("GST_MINI_OBJECT(memory)->lockstate = %X\n",GST_MINI_OBJECT(memory)->lockstate);

  gst_memory_unmap (memory, &info);

  g_print ("GST_MINI_OBJECT(memory)->refcount = %d\n", GST_MINI_OBJECT(memory)->refcount);

  gst_memory_unref (memory);
  memory = NULL;
  memset (&info, 0, sizeof(GstMapInfo));
  /**
   * 关于对齐数的问题：
   * 1. 为了让系统读取用户内存的时候可以从0x0开始，0x8等便于一次性读取内存地址（因为64位系统一次性可以读取8字节），
   *    分配内存的时候默认使用了linux下的8对齐。
   * 
   * 2. 分配内存函数会检测申请的内存地址的最后一个字节是否是 0x1 到 0x7 或者 0x9 到 0xF
   *    gst_memory_alignment 是 0x7，只要align < 0x7,align就是0x7；大于0x7,则 align |= gst_memory_alignment
   *    align |= gst_memory_alignment; 
   *    if ((aoffset = ((guintptr) data & align))) {
   *      aoffset = (align + 1) - aoffset;
   *      data += aoffset;
   *      maxsize -= aoffset;
   *    }
   */
  GstAllocationParams *params = gst_allocation_params_new ();
  params->prefix = 3;
  params->padding = 2;
  params->align = 3; /* 对齐数必须是2的幂次减一  */
  memory = gst_allocator_alloc (NULL, 4, params);
  gst_allocation_params_free (params);
  gst_memory_map (memory, &info, GST_MAP_WRITE);
  /* 这里的maxsize = 对齐size + size + 后缀size，注意没有prefix大小
   * 所以 info.maxsize = 7 + 4 + 2 = 13
   */
  g_print ("info.maxsize = %ld\n",info.maxsize);
  gst_memory_unmap (memory, &info);
  gst_memory_unref (memory);

  return 0;
}