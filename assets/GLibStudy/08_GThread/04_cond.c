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

static GCond cond;
static GMutex mutex;
static gint next;  /* next变量会被mutex上锁 */


/**
 * @brief
 * @calledby: produce_values ()
*/
static void
push_value (gint value)
{
  g_mutex_lock (&mutex);
  while (next != 0)
    g_cond_wait (&cond, &mutex);
  next = value;
  if (g_test_verbose ())
    g_printerr ("Thread %p producing next value: %d\n", g_thread_self (), value);
  if (value % 10 == 0)
    g_cond_broadcast (&cond);
  else
    g_cond_signal (&cond);
  g_mutex_unlock (&mutex);
}

/**
 * @brief
 * @calledby: consume_values ()
*/
static gint
pop_value (void)
{
  gint value;

  g_mutex_lock (&mutex);
  while (next == 0)
    g_cond_wait (&cond, &mutex);
  
  value = next;
  next = 0;
  g_cond_broadcast (&cond);
  if (value != -1)
    g_print ("Thread %p consuming value %d\n", g_thread_self (), value);
  g_mutex_unlock (&mutex);

  return value;
}


/**
 * @brief: 线程执行函数，产生value
 * @call: push_value ()
*/
static gpointer
produce_values (gpointer data) {
  gint total = 0;
  gint i;

  for (i = 1; i < 100; i++) {
    total += i;
    push_value (i);
  }

  /* -1 告诉consume_values ()退出 */
  push_value (-1);
  push_value (-1);
  
  /* 1到99的和是total == 4950 */
  g_print ("Thread %p push value %d altogether\n", g_thread_self (), total);

  return GINT_TO_POINTER (total);
}

/**
 * @brief: 线程执行函数，消耗value
 * @call: pop_value ()
*/
static gpointer
consume_values (gpointer data) {
  gint accum = 0;
  gint value;

  /* 如果pop_value返回-1，退出循环 */
  while (TRUE) {
    value = pop_value ();
    if (value == -1)
      break;
    accum += value;
  }

  g_print ("Thread %p accumulated %d\n", g_thread_self (), accum);

  return GINT_TO_POINTER (accum);
}

static GThread *producer, *consumer1, *consumer2;

static void
test_cond1 (void)
{
  gint total, acc1, acc2;

  producer = g_thread_new ("producer", produce_values, NULL);
  consumer1 = g_thread_new ("consumer1", consume_values, NULL);
  consumer2 = g_thread_new ("consumer2", consume_values, NULL);

  total = GPOINTER_TO_INT (g_thread_join (producer));
  acc1 = GPOINTER_TO_INT (g_thread_join (consumer1));
  acc2 = GPOINTER_TO_INT (g_thread_join (consumer2));

}


int
main (int argc, char *argv[]) {

  test_cond1 ();


  return 0;
}