/*************************************************************************************************************************************************
 * @filename: gstmemory_alloc.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-27-2023
 * @brief: 
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

  gst_init(&argc, &argv);

  GstMemory *memory, *sub1, *sub2;

  memory = gst_allocator_alloc (NULL, 4, NULL);

  sub1 = gst_memory_share (memory, 0, 1);

  sub2 = gst_memory_share (memory, 2, 2);

  gst_memory_is_span (sub1, sub2, NULL);

  /* clean up */
  gst_memory_unref (sub1);
  gst_memory_unref (sub2);
  gst_memory_unref (memory);

  return 0;
}