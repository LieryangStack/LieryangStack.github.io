/*************************************************************************************************************************************************
 * @filename: mutex.c
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

/* 全局变量包括静态全局变量和非静态全局变量，在没有初始化值的时候，其值自动为0 
 * 实际上定义的变量是 g__current_number_lock
 */
G_LOCK_DEFINE_STATIC (current_number);

/**
 * @name: test_macro
 * @brief: 学习 G_LOCK_DEFINE_STATIC、G_LOCK_DEFINE、G_LOCK、G_UNLOCK宏的使用
*/
static int
test_macro (void) {
  static int current_number = 0;
  int ret_val;

  /* 局部变量未被初始化是随机值，如果不进行初始化，可能出现未知问题 */
  G_LOCK_DEFINE (local_current_number);

  g_mutex_init (&G_LOCK_NAME(local_current_number));

  /* 同一个线程，同一对象两次上锁会出现死锁 */
  // G_LOCK (current_number);
  G_LOCK (current_number);
  G_LOCK (local_current_number);
  ret_val = current_number = current_number + 1;
  G_UNLOCK (current_number);
  G_UNLOCK (local_current_number);

  return ret_val;
}

/**
 * @name: test_mutex
 * @brief: 学习 g_mutex_lock、g_mutex_unlock、g_mutex_trylock、g_mutex_clear的使用
*/
static int
test_mutex (void) {
  static GMutex mutex; /*静态局部变量不需要初始化*/
  GMutex local_mutex;
  static int current_number = 0;
  int ret_val;

  g_mutex_init (&local_mutex);
  g_mutex_lock (&local_mutex);
  g_mutex_lock (&mutex);
  /* 返回FALSE，说明已经被上锁 */
  g_print ("g_mutex_trylock (&mutex) =%d\n", g_mutex_trylock (&mutex));
  ret_val = current_number = current_number + 1;

  /* 会发生错误， g_mutex_clear() called on uninitialised or locked mutex */
  // g_mutex_clear (&mutex);

  g_mutex_unlock (&mutex);
  g_mutex_unlock (&local_mutex);

  return ret_val;
}

/**
 * @name：test_heap_mutex
 * @brief：堆区创建的Mutex
*/
static int
test_heap_mutex (void) {
  /* 分配内存的时候，已经初始化成0 */
  GMutex *mutex = g_slice_alloc0 (sizeof(GMutex));
  g_mutex_lock (mutex);
  g_mutex_unlock (mutex);
  g_slice_free1 (sizeof(GMutex), mutex);
}


static void
test_mutex1 (void) {
  GMutex mutex;

  g_mutex_init (&mutex);
  g_mutex_lock (&mutex);
  g_mutex_unlock (&mutex);
  g_mutex_lock (&mutex);
  g_mutex_unlock (&mutex);
  g_mutex_clear (&mutex);
}

int
main (int argc, char *argv[]) {

  test_macro();
  test_mutex();
  test_heap_mutex();

  return 0;
}