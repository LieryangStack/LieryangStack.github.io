/*************************************************************************************************************************************************
 * @filename: recmutex.c
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


static void
test_rec_mutex1 (void)
{
  GRecMutex mutex;

  g_rec_mutex_init (&mutex);
  g_rec_mutex_lock (&mutex);
  g_rec_mutex_unlock (&mutex);
  g_rec_mutex_lock (&mutex);
  g_rec_mutex_unlock (&mutex);
  g_rec_mutex_clear (&mutex);
}

static void
test_rec_mutex2 (void)
{
  static GRecMutex mutex;

  g_rec_mutex_lock (&mutex);
  g_rec_mutex_lock (&mutex);
  g_rec_mutex_unlock (&mutex);
  g_rec_mutex_unlock (&mutex);
}

static void
test_rec_mutex3 (void)
{
  static GRecMutex mutex;
  gboolean ret;

  ret = g_rec_mutex_trylock (&mutex);
  g_assert (ret);

  ret = g_rec_mutex_trylock (&mutex);
  g_assert (ret);

  g_rec_mutex_unlock (&mutex);
  g_rec_mutex_unlock (&mutex);
}

int
main (int argc, char *argv[]) {

  test_rec_mutex1 ();
  test_rec_mutex2 ();
  test_rec_mutex3 ();

  return 0;
}