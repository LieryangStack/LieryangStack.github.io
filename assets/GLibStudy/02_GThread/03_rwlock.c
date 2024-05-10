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

static void
test_rwlock1 (void)
{
  GRWLock lock;

  g_rw_lock_init (&lock);
  g_rw_lock_writer_lock (&lock);
  g_rw_lock_writer_unlock (&lock);
  g_rw_lock_writer_lock (&lock);
  g_rw_lock_writer_unlock (&lock);
  g_rw_lock_clear (&lock);
}

static void
test_rwlock2 (void)
{
  static GRWLock lock;

  g_rw_lock_writer_lock (&lock);
  g_rw_lock_writer_unlock (&lock);
  g_rw_lock_writer_lock (&lock);
  g_rw_lock_writer_unlock (&lock);
}

static void
test_rwlock3 (void)
{
  static GRWLock lock;
  gboolean ret;

  ret = g_rw_lock_writer_trylock (&lock);
  g_assert (ret);
  ret = g_rw_lock_writer_trylock (&lock);
  g_assert (!ret);

  g_rw_lock_writer_unlock (&lock);
}

static void
test_rwlock4 (void)
{
  static GRWLock lock;

  g_rw_lock_reader_lock (&lock);
  g_rw_lock_reader_unlock (&lock);
  g_rw_lock_reader_lock (&lock);
  g_rw_lock_reader_unlock (&lock);
}

static void
test_rwlock5 (void)
{
  static GRWLock lock;
  gboolean ret;

  ret = g_rw_lock_reader_trylock (&lock);
  g_print ("ret = %d\n", ret);
  g_assert (ret);
  ret = g_rw_lock_reader_trylock (&lock);
  g_print ("ret = %d\n", ret);
  g_assert (ret);

  g_rw_lock_reader_unlock (&lock);
  g_rw_lock_reader_unlock (&lock);
}

static void
test_rwlock6 (void)
{
  static GRWLock lock;
  gboolean ret;

  g_rw_lock_writer_lock (&lock);
  ret = g_rw_lock_reader_trylock (&lock);
  g_assert (!ret);
  g_rw_lock_writer_unlock (&lock);

  g_rw_lock_reader_lock (&lock);
  ret = g_rw_lock_writer_trylock (&lock);
  g_assert (!ret);
  g_rw_lock_reader_unlock (&lock);
}


int
main (int argc, char *argv[])
{
  test_rwlock1 ();
  test_rwlock2 ();
  test_rwlock3 ();
  test_rwlock4 ();
  test_rwlock5 ();
  test_rwlock6 ();

  return g_test_run ();
}