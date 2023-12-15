#include "pm-dlist.h"
  
int
main (void) {
        PMDList *list1, *list2;

        list1 = g_object_new (PM_TYPE_DLIST, NULL);
        list2 = g_object_new (PM_TYPE_DLIST, NULL);


        PMDListPrivate *list1_priv = PM_DLIST_GET_PRIVATE (list1);
        PMDListPrivate *list2_priv = PM_DLIST_GET_PRIVATE (list1);

        g_print ("list1_priv address = %p\n", list1_priv);
        g_print ("list2_priv address = %p\n", list2_priv);

        g_object_unref (list1);
        g_object_unref (list2);
        
        return 0;
}