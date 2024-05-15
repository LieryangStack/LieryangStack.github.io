#include<stdio.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<glib.h>

#define SIZE 10
#define NUMBER_MAX 99

static gint array[SIZE] = {11, 32, 23, 83, 92, 83, 92, 80, 81, 38};

static void test_slist_1(void);
static void test_slist_2(void);
static void test_slist_3(void);
static void test_slist_4(void);
static void test_slist_5(void);
static void test_slist_6(void);
static void test_slist_7 ();

int
main(int argc, char *argv[]){

  //test_slist_1();
  // test_slist_6();

  test_slist_7 ();
  return 0;
}

static void
slist_print(GSList *st){
  g_print("Read slist Begin:\n");
  while (st){
    g_print("%d, ",GPOINTER_TO_INT(st->data));
    st = g_slist_nth(st, 1);
  }
  g_print("\nRead slist Done\n");
}

static void
test_slist_1(void){
  GSList *slist = NULL;
  GSList *st = NULL; 
  gint nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  gint i;
  
  for(i = 0; i < 10; i++){
    /* 链表末尾增加一个新的元素 */
    slist = g_slist_append(slist, &nums[i]);
  }
  st = slist;
  g_print("Read slist Begin:\n");
  while (st){
    g_print("%d, ", *(gint *)st->data);
    st = g_slist_nth(st, 1);
  }
  g_print("\nRead slist Done\n");

  /* 翻转链表 */
  g_print("Reserve Begin:\n");
  //slist = slist->next;
  slist = g_slist_reverse(slist);
  st = slist;
  while(st){
    g_print("%d, ",*(gint *)st->data);
    st = g_slist_nth(st, 1);
  }
  g_print("\nReserve Done\n");

  /*向前插入元素*/
  slist = g_slist_prepend(slist->next, &nums[0]);
  st = slist;
  g_print("Read slist Begin:\n");
  while (st){
    g_print("%d, ", *(gint *)st->data);
    st = g_slist_nth(st, 1);
  }
  g_print("\nRead slist Done\n");

  /*最后释放链表*/
  g_slist_free(slist);
}

static void
test_slist_2(void){
  GSList *slist = NULL;
  GSList *st;
  gint nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  /* 在NULL中插入1, 相当于创建了一个链表slist，并且地一个数据是1 */
  slist = g_slist_insert_before(NULL, NULL, &nums[0]);
  slist = g_slist_insert(slist, &nums[1], 1);//尾插1,链表数据：0 ， 1
  slist_print(slist);
  slist = g_slist_insert_before(slist, slist->next, &nums[3]);
  slist_print(slist);
  slist = g_slist_insert(slist, &nums[2], 1);
  slist_print(slist);
  /* 因为末尾是NULL， 在末尾插入 6 */
  slist = g_slist_insert_before(slist, NULL, &nums[6]);
  slist_print(slist);

  /* 复制链表 */
  st = g_slist_copy(slist);
  g_print("slist data pointer = %p\nst data pointer = %p", slist->data, st->data);

  g_slist_free(st);
  g_slist_free(slist);
}

static void
test_slist_3(void){
  GSList *slist = NULL;
  GSList *st = NULL;
  gint nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  gint i;

  const gpointer n = nums;

  for(i = 0; i < 10; i++){
    /* 链表末尾增加一个新的元素 */
    slist = g_slist_append(slist, &nums[i]);
    slist = g_slist_append(slist, &nums[i]);
  }

  /* 返回链表个数 */
  g_print("This GSList length is %d\n", g_slist_length(slist));
  /* 删除所有相同 */
  slist = g_slist_remove_all(slist, &nums[1]);
  /* 删除第一个相同 */
  slist = g_slist_remove(slist, &nums[2]);
  slist_print(slist);

  g_slist_free(slist);
}

static gint
find_num(gconstpointer l, gconstpointer data){
  return *(gint*)l - GPOINTER_TO_INT(data);
}

static void
test_slist_4(void){
  GSList *slist = NULL;
  GSList *st = NULL;
  gint nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  gint i;

  for(i = 0; i < 10; i++){
    slist = g_slist_append(slist, &nums[i]);
  }

  i = g_slist_index(slist, &nums[8]);
  g_print("nums[8] index = %d\n", i);

  st = g_slist_find_custom(slist, GINT_TO_POINTER(1), find_num);
  g_print("position = %d\n", g_slist_position(slist, st));
}

gint times = 0;

/**
 * @brief: 这个分类函数是从小到大排序
 * 
 * @return: 如果 p1 出现在 p2 之前，则返回负整数（这里分类函数要p1出现在p2之后，所以返回正整数）
 *         如果 p1 == p2 ，则返回 0
 *         如果 p1 出现在 p2 之后，则返回正整数
*/
static gint
sort_r(gconstpointer p1, gconstpointer p2){
  gint32 a, b;
  a = GPOINTER_TO_INT(p1);
  b = GPOINTER_TO_INT(p2);
  times++;
  g_print("a = %d b = %d   %d\n",a, b, times);
  return (a > b ? +1 : a == b ? 0 : -1);
}

/**
 * @brief: 这个分类函数是从大到小排序
 * 
 * @return: 如果 p1 出现在 p2 之前，则返回负整数
 *         如果 p1 == p2 ，则返回 0
 *         如果 p1 出现在 p2 之后，则返回正整数
*/
static gint
sort(gconstpointer p1, gconstpointer p2){
  gint32 a, b;
  a = GPOINTER_TO_INT(p1);
  b = GPOINTER_TO_INT(p2);

  return (a < b ? +1 : a == b ? 0 : -1);
}

static void
test_slist_5(void)
{
  GSList *slist = NULL;
  gint i;

  for (i = 0; i < SIZE; i++)
      slist = g_slist_append(slist, GINT_TO_POINTER(array[i]));
  slist = g_slist_sort(slist, sort_r);//使用sort函数排列链表
  slist_print(slist);

  //slist = g_slist_sort_with_data(slist, (GCompareDataFunc)sort_r, NULL);
  slist_print(slist);
  g_slist_free(slist);
}

static void
print(gpointer p1, gpointer p2){
  g_print("%d, ", GPOINTER_TO_INT(p1));
}

static void
test_slist_6(void){
  GSList *slist = NULL;
  GSList *st = NULL;
  GSList *sc = NULL;
  gint i;

  for(i = 0; i < SIZE; i++){
    /* 按排序规则插入元素 */
    slist = g_slist_insert_sorted(slist, GINT_TO_POINTER(array[i]), sort);
    st = g_slist_insert_sorted_with_data(st, GINT_TO_POINTER(array[i]), (GCompareDataFunc)sort_r, NULL);
  }

  sc = g_slist_concat(slist, st);

  g_slist_foreach(sc, print, NULL);

  g_print("\n");
}

/**
 * @brief: 如果原先的链表没有按顺序排列，最新按排序插入的节点，会在第一个符合该节点的位置插入
 * 
 * 比如现在原先的顺序是 16 13，所以现在 14 < 16 就直接插入到最前面了，他再遍历后面的
*/
static void
test_slist_7 () { 
  GSList *list = g_slist_append (NULL, GINT_TO_POINTER(16));
  g_print ("list = %p\n", list); 
  list = g_slist_append (list, GINT_TO_POINTER(13));
  g_print ("list = %p\n", list); /* 返回的是头节点 */

  list = g_slist_insert_sorted (list, GINT_TO_POINTER(14), sort_r);

  g_slist_foreach(list, print, NULL);
  G_SLIst
  g_slist_free (list);
}
