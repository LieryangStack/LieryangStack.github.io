/*************************************************************************************************************************************************
 * @filename: cond.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Dec-4-2023
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

#include <glib.h>
#include <stdio.h>

#include <gthreadprivate.h>

static GCond cond;
static GMutex mutex;


static void
cond_wait (GCond  *cond,
           GMutex *mutex)
{
  guint sampled = (guint) g_atomic_int_get (&cond->i[0]);

  g_mutex_unlock (mutex);
  g_print ("Thread %p waiting\n", g_thread_self ());
  g_futex_simple (&cond->i[0], (gsize) FUTEX_WAIT_PRIVATE, (gsize) sampled, NULL);
  g_print ("Thread %p finish\n", g_thread_self ());
  g_mutex_lock (mutex);
}

static gpointer
push_value (gpointer data)
{
  g_mutex_lock (&mutex);
  g_cond_broadcast (&cond);
  // g_cond_signal (&cond);
  g_mutex_unlock (&mutex);
}


static gpointer
pop_value (gpointer data) {

  g_mutex_lock (&mutex);
  cond_wait (&cond, &mutex);
  g_usleep (1 * G_USEC_PER_SEC);
  g_print ("Thread %p\n", g_thread_self ());
  g_mutex_unlock (&mutex);
}


static GThread *producer;

static void
test_cond1 (void)
{
  gint total, acc1, acc2;
  GThread *threads[100];

  for (int i = 0; i < 10; i++)
    threads[i] = g_thread_new (NULL, pop_value, NULL);

  g_usleep (1 * G_USEC_PER_SEC);

  producer = g_thread_new ("producer", push_value, NULL);

  for (int i = 0; i < 10; i++)
    g_thread_join (threads[i]); 

  g_thread_join (producer);
}


int
main (int argc, char *argv[]) {

  test_cond1 ();
  // test_cond2 ();
  // test_wait_until ();


  return 0;
}