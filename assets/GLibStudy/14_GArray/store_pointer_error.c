#include <glib.h>


int
main (int argc, char *argv[]) {

  GArray *garray;
  gchar *hello = "hello hello hello hello";
  gchar *word = "word";
  gchar *out_str = NULL;

  /**
 * g_array_new:
 * @zero_terminated: 如果数组在末尾应有一个额外的元素并设置为0，则为%TRUE
 * @clear_: 如果在分配时应将#GArray元素自动清除为0，则为%TRUE
 * @element_size: 每个元素的大小（以字节为单位)
 *
 * 创建一个新的具有引用计数1的#GArray。
 */
  garray = g_array_new (TRUE, TRUE, sizeof (gchar *));

  g_print ("hello4324234 = %p\n", hello);
  g_print ("word = %p\n", word);

  /* 把两个元素添加到数组里面 */
  g_array_append_vals (garray, hello, 1); /* 第一个元素超过了 8 byte,所以只能存储"hello he" */
  g_array_append_vals (garray, word, 1);

  g_print ("garray->len = %d\n", garray->len);


  g_print ("%s\n", (garray->data));

  // g_assert_cmpuint (garray->len, ==, 5);
  // g_assert_cmpstr (garray->data, ==, "hello");

  out_str = g_array_free (garray, FALSE);
  // g_assert_cmpstr (out_str, ==, "hello");
  g_free (out_str);

  return 0;
}