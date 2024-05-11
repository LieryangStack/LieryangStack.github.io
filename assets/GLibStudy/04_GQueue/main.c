#include <stdio.h>
#include <glib.h>

typedef struct _map map;
struct _map
{
  gint key;
  gchar *value;
};

map m[10] = {{0, "zero"},
             {1, "one"},
             {2, "two"},
             {3, "three"},
             {4, "four"},
             {5, "five"},
             {6, "six"},
             {7, "seven"},
             {8, "eight"},
             {9, "nine"}};

static void
myPrintInit(gpointer p1, gpointer fmt){
  g_print(fmt, *(gint *)p1);
}

static void
myPrintStr(gpointer p1, gpointer fmt){
  g_print(fmt, (gchar *)p1);
}

static void
test_queue_1(void){
  /*创建队列*/
  GQueue *queue = g_queue_new();
  /*判断队列是否为空，空返回1*/
  gboolean b = g_queue_is_empty(queue);
  g_print("The queue should be empty now.\t\tResult: %s.\n", b ? "YES" : "NO");

  gint i;
  for(i = 0; i < sizeof(m) / sizeof(m[0]); i++)
    /* 将数据data压入队列尾部 */
    g_queue_push_tail(queue, m[i].value);
  
  g_print("Now the queue[after push tail]:\n");
  /* 遍历队列 */
  g_queue_foreach(queue, myPrintStr, "%s, ");
  g_print("\n");

  /*获得队列元素*/
  g_print("The length should be '%d' now.\t\tResult: %d.\n",
          10, g_queue_get_length(queue));
  /*只查看元素，不取出*/
  g_print("head :%s\n",(gchar *)g_queue_peek_head(queue));
  g_print("tail :%s\n",(gchar *)queue->tail->data);

  g_print("3: %s\n",(gchar *)g_queue_peek_nth(queue, 3));

  /*取出两头元素，取出后，两头该元素就被删除了*/
  g_print("head :%s\n",(gchar *)g_queue_pop_head(queue));
  g_print("tail :%s\n",(gchar *)g_queue_pop_tail(queue));
  g_queue_foreach(queue, myPrintStr, "%s, ");
  g_print("\n");

  g_print("n : %s\n",(gchar *)g_queue_pop_nth(queue, 2));
  g_queue_foreach(queue, myPrintStr, "%s, ");
  g_print("\n");

  /*将数据压入队列头*/
  g_queue_push_head(queue,  m[0].value);
  /*指定位置压入*/
  g_queue_push_nth(queue, m[3].value, 3);
  g_queue_push_tail(queue,  m[9].value);
  g_queue_foreach(queue, myPrintStr, "%s, ");
  g_print("\n");

  /*释放队列*/
  g_queue_free(queue);
}

static gint
sort(gconstpointer p1, gconstpointer p2, gpointer user_data){/*正序*/
  gint32 a = *(gint*)(p1);
  gint32 b = *(gint*)(p2);
  return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2, gpointer user_data){/*正序*/
  gint32 a = *(gint*)(p1);
  gint32 b = *(gint*)(p2);
  return (a < b ? +1 : a == b ? 0 : -1);
}

static gint
myCompareInt(gconstpointer p1, gconstpointer p2){
  const gint *a = p1;
  const gint *b = p2;
  return *a - *b;
}

static void
test_queue_2(void){
  GQueue *queue = NULL;
  queue = g_queue_new();

  gint i;
  for(i = 0; i< sizeof(m)/sizeof(m[0]); i++)
    g_queue_insert_sorted(queue, &m[i].key, sort_r, NULL);
  for(i = 0; i < queue->length; i++)
    g_print("%d, ", *(gint *)g_queue_peek_nth(queue, i));
  g_print("\n");

  /*根据data删除数据*/
  g_queue_remove(queue, &m[3].key);
  for(i = 0; i < queue->length; i++)
    g_print("%d, ", *(gint *)g_queue_peek_nth(queue, i));
  g_print("\n");

  g_queue_insert_after(queue, g_queue_find_custom(queue, &m[4].key, myCompareInt), &m[3].key);
  for(i = 0; i < queue->length; i++)
    g_print("%d, ", *(gint *)g_queue_peek_nth(queue, i));
  g_print("\n");

  g_queue_free(queue);
}

int
main(int argc, char *argv[]){

  test_queue_2();

  return 0;
}

