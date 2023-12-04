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

typedef struct
{
  GMutex mutex;
  GCond  cond;
  gint   limit;
  gint   count;
} Barrier;

static void
barrier_init (Barrier *barrier,
              gint     limit)
{
  g_mutex_init (&barrier->mutex);
  g_cond_init (&barrier->cond);
  barrier->limit = limit;
  barrier->count = limit;
}

static gint
barrier_wait (Barrier *barrier)
{
  gint ret;

  g_mutex_lock (&barrier->mutex);
  barrier->count--;
  if (barrier->count == 0)
    {
      ret = -1;
      barrier->count = barrier->limit;
      g_cond_broadcast (&barrier->cond);
    }
  else
    {
      ret = 0;
      while (barrier->count != barrier->limit)
        g_cond_wait (&barrier->cond, &barrier->mutex);
    }
  g_mutex_unlock (&barrier->mutex);

  return ret;
}

static void
barrier_clear (Barrier *barrier)
{
  g_mutex_clear (&barrier->mutex);
  g_cond_clear (&barrier->cond);
}

static Barrier b;
static gint check;

static gpointer
cond2_func (gpointer data)
{
  gint value = GPOINTER_TO_INT (data);
  gint ret;

  g_atomic_int_inc (&check);

  if (g_test_verbose ())
    g_printerr ("thread %d starting, check %d\n", value, g_atomic_int_get (&check));

  g_usleep (10000 * value);

  g_atomic_int_inc (&check);

  if (g_test_verbose ())
    g_printerr ("thread %d reaching barrier, check %d\n", value, g_atomic_int_get (&check));

  ret = barrier_wait (&b);

  g_assert_cmpint (g_atomic_int_get (&check), ==, 10);

  if (g_test_verbose ())
    g_printerr ("thread %d leaving barrier (%d), check %d\n", value, ret, g_atomic_int_get (&check));

  return NULL;
}

/* this test demonstrates how to use a condition variable
 * to implement a barrier
 */
static void
test_cond2 (void)
{
  gint i;
  GThread *threads[5];

  g_atomic_int_set (&check, 0);

  barrier_init (&b, 5);
  for (i = 0; i < 5; i++)
    threads[i] = g_thread_new (NULL, cond2_func, GINT_TO_POINTER (i));

  for (i = 0; i < 5; i++)
    g_thread_join (threads[i]);

  g_assert_cmpint (g_atomic_int_get (&check), ==, 10);

  barrier_clear (&b);
}

static void
test_wait_until (void)
{
  gint64 until;
  GMutex lock;
  GCond local_cond;

  /* This test will make sure we don't wait too much or too little.
   *
   * We check the 'too long' with a timeout of 60 seconds.
   *
   * We check the 'too short' by verifying a guarantee of the API: we
   * should not wake up until the specified time has passed.
   */
  g_mutex_init (&lock);
  g_cond_init (&local_cond);

  until = g_get_monotonic_time () + G_TIME_SPAN_SECOND;

  /* Could still have spurious wakeups, so we must loop... */
  g_mutex_lock (&lock);
  while (g_cond_wait_until (&local_cond, &lock, until))
    ;
  g_mutex_unlock (&lock);

  /* Make sure it's after the until time */
  g_assert_cmpint (until, <=, g_get_monotonic_time ());

  /* Make sure it returns FALSE on timeout */
  until = g_get_monotonic_time () + G_TIME_SPAN_SECOND / 50;
  g_mutex_lock (&lock);
  g_assert (g_cond_wait_until (&local_cond, &lock, until) == FALSE);
  g_mutex_unlock (&lock);

  g_mutex_clear (&lock);
  g_cond_clear (&local_cond);
}



int
main (int argc, char *argv[]) {

  test_cond1 ();
  // test_cond2 ();
  // test_wait_until ();


  return 0;
}