#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

static guint
my_hash (gconstpointer key) {

  g_print ("key address = %d\n", GPOINTER_TO_UINT (key));
  g_print ("key = %s, hash value = %d\n", (const gchar*)key, (guint) *((const gint*) key));

  return (guint) *((const gint*) key);
}

static gboolean
my_hash_equal (gconstpointer a,
               gconstpointer b)
{
  return *((const gint*) a) == *((const gint*) b);
}

int
main (int argc, char *argv[]) {

  /**
 * @brief: 
 * @hash_func: a function to create a hash value from a key
 * @key_equal_func: a function to check two keys for equality
 *
 * Creates a new #GHashTable with a reference count of 1.
 *
 * Hash values returned by @hash_func are used to determine where keys are stored within the #GHashTable data structure. The g_direct_hash(), g_int_hash(), g_int64_hash(), g_double_hash() and g_str_hash()
 * functions are provided for some common types of keys.
 * If @hash_func is %NULL, g_direct_hash() is used.
 *
 * @key_equal_func is used when looking up keys in the #GHashTable.
 * The g_direct_equal(), g_int_equal(), g_int64_equal(), g_double_equal()
 * and g_str_equal() functions are provided for the most common types
 * of keys. If @key_equal_func is %NULL, keys are compared directly in
 * a similar fashion to g_direct_equal(), but without the overhead of
 * a function call. @key_equal_func is called with the key from the hash table
 * as its first parameter, and the user-provided key to check against as
 * its second.
 *
 * Returns: a new #GHashTable
 * 
 * 
*创建一个引用计数为1的新 #GHashTable。

*由 @hash_func 返回的哈希值用于确定将键存储在 #GHashTable 数据结构中的位置。对于一些常见类型的键，提供了 g_direct_hash()、g_int_hash()、g_int64_hash()、g_double_hash() 和 g_str_hash() 函数。如果 @hash_func 为 %NULL，则使用 g_direct_hash()。

*@key_equal_func 在查找 #GHashTable 中的键时使用。对于最常见类型的键，提供了 g_direct_equal()、g_int_equal()、g_int64_equal()、g_double_equal() 和 g_str_equal() 函数。如果 @key_equal_func 为 %NULL，则键会以类似于 g_direct_equal() 的方式直接比较，但没有函数调用的开销。@key_equal_func 被调用时，哈希表中的键作为其第一个参数，要检查的用户提供的键作为其第二个参数。
 */
  GHashTable *hash_table = g_hash_table_new (my_hash, my_hash_equal);

  my_hash("lieryang");

  my_hash ("test");


  g_hash_table_unref (hash_table);

  return 0;
}