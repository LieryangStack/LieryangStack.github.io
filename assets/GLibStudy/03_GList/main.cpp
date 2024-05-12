#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

static gint
sort(gconstpointer p1, gconstpointer p2){
  gint32 a, b;
  a = GPOINTER_TO_INT(p1);
  b = GPOINTER_TO_INT(p2);

  return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2){
  gint32 a, b;
  a = GPOINTER_TO_INT(p1);
  b = GPOINTER_TO_INT(p2);

  return (a < b ? +1 : a == b ? 0 : -1);
}

static void
print(gpointer p1, gpointer p2){
  g_print("%d, ", *(gint *)p1);
}

void
list_print(GList *list){
  list = g_list_first(list);
  while (list){
    g_print("%d ", GPOINTER_TO_INT(list->data));
    if(!list->next)
      break;
    /* 从给点的节点，数 1 个位置，返回其节点 */
    list = g_list_nth(list, 1);
  }
  g_print("\n");
}

int
main(int argc, char *argv[]){

  g_list_free (NULL);

  GList *list = NULL;
  gint nums[10], i;
  for( i = 0; i < 10; i++){
    nums[i] = i;
    /* 返回头结点地址 */
    
    if ( i == 3) {
      g_print("Before list pointer = %p ", list);
      list = g_list_append(list->prev, GINT_TO_POINTER(nums[i]));
      g_print("After list pointer = %p\n", list);
    } else {
      g_print("Before list pointer = %p ", list);
      list = g_list_append(list, GINT_TO_POINTER(nums[i]));
      g_print("After list pointer = %p\n", list);
    }
  }

  g_print ("heard list pre = %p\n", list->prev);

  /* 头加 */
  list = g_list_prepend(list, GINT_TO_POINTER(nums[9]));
  list_print(list);
  g_print("%p\n", list);
  /*得到头结点*/
  list = g_list_first(list);
  g_print("head = %p\n", list);
  list = g_list_last(list);
  g_print("tail = %p\n", list);

  /* position = 0 表示在之前插入 */
  list = g_list_insert(list, GINT_TO_POINTER(nums[0]), 0);
  list_print(list);

  g_list_free(list);

  return 0;
}
