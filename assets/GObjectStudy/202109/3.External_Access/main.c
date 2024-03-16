#include "pm-dlist.h"

int
main (void)
{
  GValue list1_tail = {0};
  GValue list2_head = {0};

  /* GObject 库的类型管理系统的初始化 */
  g_type_init ();

  PMDList *list1, *list2;
  gpointer *head, *tail;
  list1 = g_object_new (PM_TYPE_DLIST,
                        "head", NULL,
                        "tail", NULL,
                        NULL);
  list2 = g_object_new (PM_TYPE_DLIST,
                        "head", NULL,
                        "tail", NULL,
                        NULL);

  g_value_init(&list1_tail, G_TYPE_POINTER);
  g_value_init(&list2_head, G_TYPE_POINTER);

  g_value_set_pointer(&list1_tail, list1);
  g_value_set_pointer(&list2_head, list2);

  g_object_set_property(G_OBJECT(list2), "head", &list1_tail);
  g_object_set_property(G_OBJECT(list1), "tail", &list2_head);

  g_object_get(list1, "head", &head, "tail", &tail, NULL);

  g_printf("%p\n", head);
  g_printf("%p\n", tail);
  
  g_object_set(list1, "head", 0x22, "tail", 0x33, NULL);
  g_object_get(list1, "head", &head, "tail", &tail, NULL);

  //g_printf("%p\n", g_value_get_pointer(&list1_tail));
  g_printf("%p\n", head);
  g_printf("%p\n", tail);

  g_value_unset(&list1_tail);
  g_value_unset(&list2_head);

  g_object_unref (list1);
  g_object_unref (list2);

  return 0;
}