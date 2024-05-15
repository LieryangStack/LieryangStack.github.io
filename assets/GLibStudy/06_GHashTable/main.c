#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

typedef struct _TEST_GHashTable TEST_GHashTable;
struct _TEST_GHashTable
{
  gsize            size;
  gint             mod;
  guint            mask;
  guint            nnodes;
  guint            noccupied;  /* nnodes + tombstones */

  guint            have_big_keys : 1;
  guint            have_big_values : 1;

  gpointer         keys;
  guint           *hashes;
  gpointer         values;

  GHashFunc        hash_func;
  GEqualFunc       key_equal_func;
  gatomicrefcount  ref_count;
#ifndef G_DISABLE_ASSERT
  /*
   * Tracks the structure of the hash table, not its contents: is only
   * incremented when a node is added or removed (is not incremented
   * when the key or data of a node is modified).
   */
  int              version;
#endif
  GDestroyNotify   key_destroy_func;
  GDestroyNotify   value_destroy_func;
};

static guint
my_hash (gconstpointer key) {
  g_print ("%s\n", __func__);
  return (guint) *((const gint*) key);
}

static gboolean
my_hash_equal (gconstpointer a,
               gconstpointer b) {
  g_print ("%s\n", __func__);
  return *((const gint*) a) == *((const gint*) b);
}

int
main (int argc, char *argv[]) {

/**
 * @brief: 创建一个引用计数为1的新 #GHashTable。
 * @hash_func: 这个函数是通过key创建哈希索引值的函数
 *             由 @hash_func 返回的哈希索引值用于确定将键存储在 #GHashTable 数据结构中的位置。
 *             对于一些常见类型的键，提供了 g_direct_hash()、g_int_hash()、g_int64_hash()、g_double_hash() 和 g_str_hash() 函数。
 *             如果 @hash_func 为 %NULL，则使用 g_direct_hash()。
 * 
 * @key_equal_func: 在查找 #GHashTable 中的键时使用。
 *                  对于最常见类型的键，提供了 g_direct_equal()、g_int_equal()、g_int64_equal()、g_double_equal() 和 g_str_equal() 函数。
 *                  如果 @key_equal_func 为 %NULL，则键会以类似于 g_direct_equal() 的方式直接比较，但没有函数调用的开销。
 *                  @key_equal_func 被调用时，哈希表中的键作为其第一个参数，要检查的用户提供的键作为其第二个参数。
 *
 *
 * @returns: a new #GHashTable
 * 
 */
  GHashTable *hash_table = g_hash_table_new (my_hash, my_hash_equal);

  /* 插入的时候会调用 @hash_func 函数，获取 "han" 这个键，对应的哈希索引值 */
  g_hash_table_insert (hash_table, "han", "hanzidehan");
  /* 插入的时候会调用 @hash_func 函数，获取 "zi" 这个键，对应的哈希索引值 */
  g_hash_table_insert (hash_table, "zi", "hanzidezi");

  /* 查找的时候，先调用 @hash_func 函数获取 key 对应的哈希索引值，然后通过哈希索引值找到对应的键，最后调用@key_equal_func，判断两个key是否相等 */
  gchar *str = g_hash_table_lookup (hash_table, "han");
  g_print ("key=han, value=%s\n", str);

  str = g_hash_table_lookup (hash_table, "zi");
  g_print ("key=zi, value=%s\n", str);

  /* 创建的时候ref_count == 1，除了创建函数，没有其他函数能修改ref_count，所以此时解引用一次就可以释放所有内存 */
  g_print ("ref_count = %d\n", ((TEST_GHashTable *)hash_table)->ref_count);
  g_hash_table_unref (hash_table);
  

  return 0;
}