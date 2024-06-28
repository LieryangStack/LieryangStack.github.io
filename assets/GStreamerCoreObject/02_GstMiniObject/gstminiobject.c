#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gst/gst_private.h"
#include "gst/gstminiobject.h"
#include "gst/gstinfo.h"
#include <gobject/gvaluecollector.h>

GType _gst_mini_object_type = 0;

/* Mutex used for weak referencing */
G_LOCK_DEFINE_STATIC (qdata_mutex);
static GQuark weak_ref_quark;

/* 0x10000   十进制65536*/
#define SHARE_ONE (1 << 16) 
/* 0x20000  十进制131072*/
#define SHARE_TWO (2 << 16) 
/* 0 */
#define SHARE_MASK (~(SHARE_ONE - 1)) 
/* state要大于0x20000才成立 */
#define IS_SHARED(state) (state >= SHARE_TWO) 
/* 0x100 */
#define LOCK_ONE (GST_LOCK_FLAG_LAST) 
/* 0xFF 十进制255 */
#define FLAG_MASK (GST_LOCK_FLAG_LAST - 1) 
/* 0xFF00 十进制65280 */
#define LOCK_MASK ((SHARE_ONE - 1) - FLAG_MASK)
/* 0xFFFF */
#define LOCK_FLAG_MASK (SHARE_ONE - 1) 


/**
 * GST_TYPE_MINI_OBJECT:
 *
 * 与 #GstMiniObject 相关联的 #GType。
 *
 * 自版本 1.20 起可用。
 */

/* 出于向后兼容的原因，我们在 GstMiniObject 结构中
 * 使用 guint 和 gpointer，以一种相当复杂的方式来存储父对象和 qdata。
 * 最初，它们仅仅是 qdata 的数量和 qdata 本身。
 *
 * guint 被用作一个原子状态整数，具有以下状态：
 * - Locked：0，基本上是一个自旋锁（忙等待，不会挂起该线程）
 * - No parent，无 qdata：1（指针为 NULL）
 * - 一个父对象：2（指针包含父对象）
 * - 多个父对象或 qdata：3（指针包含一个 PrivData 结构体）
 *
 * 除非我们处于状态 3，否则我们总是必须原子性地移动到锁定状态，
 * 并在稍后再将其释放回目标状态，以便在访问指针时使用。
 * 当我们处于状态 3 时，我们将不再转移到更低的状态
 *
 * FIXME 2.0：我们应该直接在结构体内部存储这些信息，
 * 可能直接为几个父对象保留空间
 */

/* 私有数据的三种状态 */
enum {
  PRIV_DATA_STATE_LOCKED = 0, /* 被锁定状态 */
  PRIV_DATA_STATE_NO_PARENT = 1, /* 没有父对象 */
  PRIV_DATA_STATE_ONE_PARENT = 2, /* 一个父对象 */
  /* 多个父对象或者QData,注意，也有可能只有一个父对象或者没有父对象，但是已经具备存储多个父对象空间了 */
  PRIV_DATA_STATE_PARENTS_OR_QDATA = 3, 
};


typedef struct {
  GQuark quark;
  GstMiniObjectNotify notify;
  gpointer data;
  GDestroyNotify destroy;
} GstQData;

typedef struct {
  /* Atomic spinlock: 1 if locked, 0 otherwise */
  gint parent_lock;
  guint n_parents, n_parents_len;
  GstMiniObject **parents;

  guint n_qdata, n_qdata_len;
  GstQData *qdata;
} PrivData;

#define QDATA(q,i)          (q->qdata)[(i)]
#define QDATA_QUARK(o,i)    (QDATA(o,i).quark)
#define QDATA_NOTIFY(o,i)   (QDATA(o,i).notify)
#define QDATA_DATA(o,i)     (QDATA(o,i).data)
#define QDATA_DESTROY(o,i)  (QDATA(o,i).destroy)

/* 实际上是 G_DEFINE_BOXED_TYPE */
GST_DEFINE_MINI_OBJECT_TYPE (GstMiniObject, gst_mini_object);

/**
 * @brief: 注册GstMiniObject对象
 * @calledby: gst_init()函数
*/
void
_priv_gst_mini_object_initialize (void)
{
  _gst_mini_object_type = gst_mini_object_get_type ();
  weak_ref_quark = g_quark_from_static_string ("GstMiniObjectWeakRefQuark");
}

/**
 * gst_mini_object_init: (skip)
 * @brief: 用所需要的 copy、dispose、free 函数初始化一个GstMiniObject对象
 * @mini_object: a #GstMiniObject
 * @flags: initial #GstMiniObjectFlags
 * @type: the #GType of the mini-object to create
 * @copy_func: (allow-none): the copy function, or %NULL
 * @dispose_func: (allow-none): the dispose function, or %NULL
 * @free_func: (allow-none): the free function or %NULL
 */
void
gst_mini_object_init (GstMiniObject * mini_object, guint flags, GType type,
    GstMiniObjectCopyFunction copy_func,
    GstMiniObjectDisposeFunction dispose_func,
    GstMiniObjectFreeFunction free_func)
{
  mini_object->type = type;
  mini_object->refcount = 1;
  mini_object->lockstate = 0;
  mini_object->flags = flags;

  mini_object->copy = copy_func;
  mini_object->dispose = dispose_func;
  mini_object->free = free_func;

  /* 默认设定为没有父对象状态 */
  g_atomic_int_set ((gint *) & mini_object->priv_uint,
      PRIV_DATA_STATE_NO_PARENT);
  mini_object->priv_pointer = NULL;

  GST_TRACER_MINI_OBJECT_CREATED (mini_object);
}


/**
 * @brief: 拷贝一个GstMiniObject对象
 * @return: 如果复制是可能的，则返回新的GstMiniObject对象，否则返回NULL
*/
GstMiniObject *
gst_mini_object_copy (const GstMiniObject * mini_object)
{
  GstMiniObject *copy;

  g_return_val_if_fail (mini_object != NULL, NULL);

  if (mini_object->copy)
    copy = mini_object->copy (mini_object);
  else
    copy = NULL;

  return copy;
}


/**
 * @brief: 用指定的GstLockFlags锁定GstMiniObject对象的访问状态
 * @note: lockstate是一个gint变量，该变量具有4个字节
 *        第一个字节：标记读写状态 0x01是读，0x02是
 *        第二个字节是：上了几个锁 LOCK_ONE （有几个对象上了锁） 0x100
 *        第三个字节是：GST_LOCK_FLAG_EXCLUSIVE有几个 0x10000
 * @return: 如果被锁成功，返回TRUE
*/
gboolean
gst_mini_object_lock (GstMiniObject * object, GstLockFlags flags)
{
  guint access_mode, state, newstate;

  /* 检测object指针是否为NULL */
  g_return_val_if_fail (object != NULL, FALSE);
  /* 检查object对象可否能够进行上锁 */
  g_return_val_if_fail (GST_MINI_OBJECT_IS_LOCKABLE (object), FALSE);

  /* 如果该对象初始化flag就是只读，现在又要进行写锁，则返回FALSE */
  if (G_UNLIKELY (object->flags & GST_MINI_OBJECT_FLAG_LOCK_READONLY &&
          flags & GST_LOCK_FLAG_WRITE))
    return FALSE;

  do {
    /* 获取进行锁的模式, FLAG_MASK就是0xFF */
    access_mode = flags & FLAG_MASK;
    /* 获取该对象目前锁的状态，初始化的时候 lockstate = 0;  */
    newstate = state = (guint) g_atomic_int_get (&object->lockstate);

    GST_CAT_TRACE (GST_CAT_LOCKING, "lock %p: state %08x, access_mode %u",
        object, state, access_mode);

    /* 如果传入参数独有锁，则执行 */
    if (access_mode & GST_LOCK_FLAG_EXCLUSIVE) {
      /* shared ref，读写独有锁占用的是Flag的前八位，共享引用计数占用的是16位以后 */  
      newstate += SHARE_ONE; /* newstate = newstate + 0x10000*/
      access_mode &= ~GST_LOCK_FLAG_EXCLUSIVE; /* access_mode 已经获取了独有锁flag，去掉独有锁，查看还剩下什么flag */
    }

    /**
     * 如果对象已经处于写锁状态或者请求写锁，而且
     * shared 计数 >= 2 ，上锁失败
    */
    if (((state & GST_LOCK_FLAG_WRITE) != 0
            || (access_mode & GST_LOCK_FLAG_WRITE) != 0)
        && IS_SHARED (newstate))
      goto lock_failed;
    

    if (access_mode) {
      if ((state & LOCK_FLAG_MASK) == 0) { /* 该对象没有处于任何锁状态，也就是 state = 0 */
        /* nothing mapped, set access_mode */
        newstate |= access_mode; /* 把请求上的锁，赋值给 newstate */
      } else {
        /* access_mode must match */
        if ((state & access_mode) != access_mode) /* 具有写锁的时候不能上读锁，具有读锁的时候不能上写锁*/
          goto lock_failed;
      }
      /* increase refcount */
      newstate += LOCK_ONE;
    }
  } while (!g_atomic_int_compare_and_exchange (&object->lockstate, state,
          newstate));

  return TRUE;

lock_failed:
  {
    GST_CAT_DEBUG (GST_CAT_LOCKING,
        "lock failed %p: state %08x, access_mode %u", object, state,
        access_mode);
    return FALSE;
  }
}

/**
 * @brief: 用指定的GstLockFlags解除GstMiniObject对象的访问状态
*/
void
gst_mini_object_unlock (GstMiniObject * object, GstLockFlags flags)
{
  guint access_mode, state, newstate;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GST_MINI_OBJECT_IS_LOCKABLE (object));

  do {
    access_mode = flags & FLAG_MASK;
    newstate = state = (guint) g_atomic_int_get (&object->lockstate);

    GST_CAT_TRACE (GST_CAT_LOCKING, "unlock %p: state %08x, access_mode %u",
        object, state, access_mode);
    
    /* 先去除独有锁的标记 */
    if (access_mode & GST_LOCK_FLAG_EXCLUSIVE) {
      /* shared counter */
      g_return_if_fail (state >= SHARE_ONE);
      newstate -= SHARE_ONE;
      access_mode &= ~GST_LOCK_FLAG_EXCLUSIVE; /* 去除传入参数中的独有锁 */
    }

    if (access_mode) {
      g_return_if_fail ((state & access_mode) == access_mode);
      /* decrease the refcount */
      newstate -= LOCK_ONE;
      /* last refcount, unset access_mode */
      if ((newstate & LOCK_FLAG_MASK) == access_mode) /* 如果是最后一个锁引用，把第一第二个字节置零 */
        newstate &= ~LOCK_FLAG_MASK;
    }
  } while (!g_atomic_int_compare_and_exchange (&object->lockstate, state,
          newstate));
}

/***
 * @brief: 如果 GstMiniObject 对象是 PRIV_DATA_STATE_PARENTS_OR_QDATA，直接返回
 *         如果不是 PRIV_DATA_STATE_PARENTS_OR_QDATA，一直等待被解锁，然后再切换到锁状态，返回锁之前的状态
 * 
 * @details:
 * 首先，object->priv_uint == PRIV_DATA_STATE_PARENTS_OR_QDATA，直接返回 PRIV_DATA_STATE_PARENTS_OR_QDATA
 * 循环体：
 *        循环检测是否等于 PRIV_DATA_STATE_PARENTS_OR_QDATA， 如果是，直接退出循环，返回 PRIV_DATA_STATE_PARENTS_OR_QDATA
 *        循环体不等于 PRIV_DATA_STATE_PARENTS_OR_QDATA 
 *             如果 object->priv_uint == PRIV_DATA_STATE_LOCKED 进入 死循环状态，一直检测是否是其他状态
 *                  如果 object->priv_uint 不是锁状态，切换到锁状态，退出循环，返回 锁之前的状态
*/
static guint
lock_priv_pointer (GstMiniObject * object) {
  gint priv_state = g_atomic_int_get ((gint *) & object->priv_uint);

  /* 如果不是多个父对象或者QData，则执行 */
  if (priv_state != PRIV_DATA_STATE_PARENTS_OR_QDATA) {

    /**
     * 只要GstMiniObject对象不是多个父对象或者QData状态
     * 也就是说只要 GstMiniObject处于没有父对象或者一个父对象状态
     * 我们就是去设定GstMiniObject处于锁状态PRIV_DATA_STATE_LOCKED
     * 
     * g_atomic_int_compare_and_exchange函数有三个参数，依次为 当前值 旧值 新值
     * 如果整数变量的当前值与提供的“旧值”相同，导致变量值被成功更新为“新值”，函数返回 TRUE
     * 如果整数变量的当前值与提供的“旧值”不同，变量值不会被改变，函数返回 FALSE。(意味着被其他线程修改了object->priv_uint)
    */
    while (priv_state != PRIV_DATA_STATE_PARENTS_OR_QDATA &&
        (priv_state == PRIV_DATA_STATE_LOCKED ||
            !g_atomic_int_compare_and_exchange ((gint *) & object->priv_uint,
                priv_state, PRIV_DATA_STATE_LOCKED)))
      
      /* 如果整数变量的当前值与提供的“旧值”不同，变量值不会被改变，函数返回 FALSE。才会执行该程序 */
      priv_state = g_atomic_int_get ((gint *) & object->priv_uint);

      /* 注意，如果我们得到了完整的结构体，我们没有存储
         PRIV_DATA_STATE_LOCKED 并且实际上没有锁定 priv 指针 */

      /**
       * 这段注释指出，在某种情况下，即使获得了完整的结构体，代码并没有将相关的状态设置为 PRIV_DATA_STATE_LOCKED，
       * 也没有实际上锁定 priv 指针。这可能意味着在这种特定的情形下，锁定操作被认为是不必要的，因为已经安全地获得了结构体的完整内容。
       * 此注释提醒开发者或维护者注意这个特殊的逻辑处理方式，以便于理解代码的行为和确保正确的同步或互斥操作。
      */
  }

  return priv_state;
}

/**
 * 不可写状态：
 *      1.这是一个能够上锁的GstMiniObject对象
 *        如果被上两次独有锁，也就是处于两次共享状态，直接返回不可写
 *      2.这是一个只读的GstMiniObject对象，引用计数不等于1，直接返回不可写
 * 
 * 可写状况： 对象可锁，只有一个用户上独有锁或者对象只读，引用计数等于 1 的时候
 *         1. 只有一个用户上独有锁，只有一个父对象且父对象本身是可写的，返回可写
 *         2. 只有一个用户上独有锁，没有父对象，返回可写。
 * 
*/
/**
 * gst_mini_object_is_writable:
 * @mini_object: 要检查的GstMiniObject对象
 *
 * 如果 @mini_object 设置了 LOCKABLE 标志，检查当前对 @object 的独占锁定是否是唯一的，
 * 这意味着对对象的更改不会对其他任何对象可见。
 *
 * 如果没有设置 LOCKABLE 标志，检查 @mini_object 的引用计数是否正好为 1，
 * 这意味着没有其他对该对象的引用，因此该对象是可写的。
 *
 * 修改GstMiniObject对象应该只在确认它是可写的之后进行。
 *
 * 返回：如果对象是可写的，则为 %TRUE。
 */
gboolean
gst_mini_object_is_writable (const GstMiniObject * mini_object)
{
  gboolean result;
  gint priv_state;

  g_return_val_if_fail (mini_object != NULL, FALSE);

  /* 检查 GstMiniObject 对象创建的时候，是否标记了能够锁的Flag */
  if (GST_MINI_OBJECT_IS_LOCKABLE (mini_object)) { /* 创建的时候是：GST_MINI_OBJECT_FLAG_LOCKABLE */
    /* 是否处于被共享状态（两个用户进行独有锁） */
    result = !IS_SHARED (g_atomic_int_get (&mini_object->lockstate));
  } else { /* 创建的时候是其他Flag */
    result = (GST_MINI_OBJECT_REFCOUNT_VALUE (mini_object) == 1);
  }

  /**
   * 不可写状态：
   *      1.这是一个能够上锁的GstMiniObject对象
   *        如果被上两次独有锁，也就是处于两次共享状态，直接返回不可写
   *      2.这是一个只读的GstMiniObject对象，引用计数不等于1，直接返回不可写
   * 
   * 可写状况： 对象可锁，只有一个用户上独有锁或者对象只读，引用计数等于 1 的时候
   *         1. 只有一个用户上独有锁，只有一个父对象且父对象本身是可写的，返回可写
   *         2. 只有一个用户上独有锁，没有父对象，返回可写。
   * 
  */
  if (!result) /* 如果处于共享状态，那就一定是可写的，直接返回 FALSE */
    return result;
  
  /* 如果不是 PRIV_DATA_STATE_PARENTS_OR_QDATA，堵塞等待可切换到 PRIV_DATA_STATE_LOCKED状态， 返回的 priv_state 是锁之前的状态*/
  priv_state = lock_priv_pointer (GST_MINI_OBJECT_CAST (mini_object));

    /* 现在我们要么需要检查完整的结构体以及其中的所有父对象，
     * 要么如果确实只有一个父对象，我们可以检查那一个 */
  if (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    PrivData *priv_data = mini_object->priv_pointer;

    /* Lock parents（ parent_lock 对应 1 是 锁， 0 不是锁 ） */
    while (!g_atomic_int_compare_and_exchange (&priv_data->parent_lock, 0, 1));
    
    /**
     * @note: 如果我们有一个父对象，我们只有在那个父对象是可写的情况下才是可写的。
     *        我们没有父对象，我们就是可写的
     *        否则，如果我们有多个父对象，我们就不是可写的
    */
    if (priv_data->n_parents == 1)
      result = gst_mini_object_is_writable (priv_data->parents[0]); /* 对应基本概念中的：只有一个父对象且该父对象本身是可写的时，对象可写*/
    else if (priv_data->n_parents == 0)
      result = TRUE;
    else
      result = FALSE;

    /* Unlock again（恢复到锁之前的状态） */
    g_atomic_int_set (&priv_data->parent_lock, 0);
  } else {
    if (priv_state == PRIV_DATA_STATE_ONE_PARENT) {
      result = gst_mini_object_is_writable (mini_object->priv_pointer);
    } else {
      g_assert (priv_state == PRIV_DATA_STATE_NO_PARENT);
      result = TRUE;
    }

    /* Unlock again （恢复到锁之前的状态） */
    g_atomic_int_set ((gint *) & mini_object->priv_uint, priv_state);
  }

  return result;
}

/**
 * gst_mini_object_make_writable: (跳过)
 * @mini_object: (完全转移): 要使其可写的GstMiniObject对象
 *
 * 检查一个对象是否可写。如果不可写，将创建并返回一个可写的副本。
 * 这会放弃对原始GstMiniObject对象的引用，并返回对新对象的引用。
 *
 * 多线程安全
 *
 * 返回：(完全转移) (可空)：一个可写的GstMiniObject对象（可能与 @mini_object 相同，也可能不同）
 *     或者如果需要复制但不可能时返回 %NULL。
 */

GstMiniObject *
gst_mini_object_make_writable (GstMiniObject * mini_object)
{
  GstMiniObject *ret;

  g_return_val_if_fail (mini_object != NULL, NULL);

  if (gst_mini_object_is_writable (mini_object)) {
    ret = mini_object;
  } else {
    ret = gst_mini_object_copy (mini_object);
    GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "copy %s miniobject %p -> %p",
        g_type_name (GST_MINI_OBJECT_TYPE (mini_object)), mini_object, ret);
    gst_mini_object_unref (mini_object);
  }

  return ret;
}

/**
 * gst_mini_object_ref: (跳过)
 * @mini_object: GstMiniObject对象
 *
 * 增加GstMiniObject对象的引用计数。
 *
 * 注意，引用计数会影响 @mini_object 的可写性，
 * 参见 gst_mini_object_is_writable()。需要特别注意的是，
 * 保留对 GstMiniObject 实例的额外引用可能会增加
 * 管道中的 memcpy 操作数量，尤其是当 miniobject 是一个 #GstBuffer 时。
 *
 * 返回：(完全转移)：GstMiniObject对象。
 */
GstMiniObject *
gst_mini_object_ref (GstMiniObject * mini_object)
{
  gint old_refcount, new_refcount;

  g_return_val_if_fail (mini_object != NULL, NULL);
  /* we can't assert that the refcount > 0 since the _free functions
   * increments the refcount from 0 to 1 again to allow resurrecting
   * the object
   g_return_val_if_fail (mini_object->refcount > 0, NULL);
   */

  old_refcount = g_atomic_int_add (&mini_object->refcount, 1);
  new_refcount = old_refcount + 1;

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "%p ref %d->%d", mini_object,
      old_refcount, new_refcount);

  GST_TRACER_MINI_OBJECT_REFFED (mini_object, new_refcount);

  return mini_object;
}

/**
 * @brief: 根据quark查找是qdata索引
 * @param match_notify: false 不需要查看传入的notify与存储的notify，传入的data与存储qdata是否相等
 *                      true 需要查看传入的notify与存储的notify，传入的data与存储qdata是否相等
*/
static gint
find_notify (GstMiniObject * object, GQuark quark, gboolean match_notify,
    GstMiniObjectNotify notify, gpointer data)
{
  guint i;
  gint priv_state = g_atomic_int_get ((gint *) & object->priv_uint);
  PrivData *priv_data;

  if (priv_state != PRIV_DATA_STATE_PARENTS_OR_QDATA)
    return -1;

  priv_data = object->priv_pointer;

  for (i = 0; i < priv_data->n_qdata; i++) {
    if (QDATA_QUARK (priv_data, i) == quark) {
      /* check if we need to match the callback too */
      if (!match_notify || (QDATA_NOTIFY (priv_data, i) == notify &&
              QDATA_DATA (priv_data, i) == data))
        return i;
    }
  }
  return -1;
}

static void
remove_notify (GstMiniObject * object, gint index)
{
  gint priv_state = g_atomic_int_get ((gint *) & object->priv_uint);
  PrivData *priv_data;

  g_assert (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA);
  priv_data = object->priv_pointer;

  /* remove item */
  priv_data->n_qdata--;
  if (priv_data->n_qdata == 0) {
    /* we don't shrink but free when everything is gone */
    g_free (priv_data->qdata);
    priv_data->qdata = NULL;
    priv_data->n_qdata_len = 0;
  } else if (index != priv_data->n_qdata) {
    QDATA (priv_data, index) = QDATA (priv_data, priv_data->n_qdata);
  }
}

/**
 * @brief: 给结构体 PrivData 分配内存空间
 * @calledby: gst_mini_object_add_parent: 只有在一个父对象的情况，ensure_priv_data 才会被调用。
 * @note: 1. 如果 priv_data 没有处于 PRIV_DATA_STATE_PARENTS_OR_QDATA 就给结构体分配空间
 *        2. 所以此时可能有一个parent，或者没有parent
*/
static void
ensure_priv_data (GstMiniObject * object)
{
  gint priv_state;
  PrivData *priv_data;
  GstMiniObject *parent = NULL;

  GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
      "allocating private data %s miniobject %p",
      g_type_name (GST_MINI_OBJECT_TYPE (object)), object);

  priv_state = lock_priv_pointer (object);
  if (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA)
    return;

  /* 现在我们要么已经锁定，要么有人在我们之前已经分配了结构体
    * 我们可以直接继续进行
    *
    * 注意：如果与此同时有其他人分配了它，我们不需要解锁，
    * 因为我们没有锁定！ */
  if (priv_state != PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    if (priv_state == PRIV_DATA_STATE_ONE_PARENT)
      parent = object->priv_pointer;

    object->priv_pointer = priv_data = g_new0 (PrivData, 1);

    if (parent) {
      /* 分配了16个 GstMiniObject *指针 */
      priv_data->parents = g_new (GstMiniObject *, 16);
      priv_data->n_parents_len = 16;
      priv_data->n_parents = 1;
      priv_data->parents[0] = parent;
    }

    /* Unlock */
    g_atomic_int_set ((gint *) & object->priv_uint,
        PRIV_DATA_STATE_PARENTS_OR_QDATA);
  }
}

static void
set_notify (GstMiniObject * object, gint index, GQuark quark,
    GstMiniObjectNotify notify, gpointer data, GDestroyNotify destroy)
{
  PrivData *priv_data;

  ensure_priv_data (object);
  priv_data = object->priv_pointer;

  if (index == -1) {
    /* add item */
    index = priv_data->n_qdata++;
    if (index >= priv_data->n_qdata_len) {
      priv_data->n_qdata_len *= 2;
      if (priv_data->n_qdata_len == 0)
        priv_data->n_qdata_len = 16;

      priv_data->qdata =
          g_realloc (priv_data->qdata,
          sizeof (GstQData) * priv_data->n_qdata_len);
    }
  }

  QDATA_QUARK (priv_data, index) = quark;
  QDATA_NOTIFY (priv_data, index) = notify;
  QDATA_DATA (priv_data, index) = data;
  QDATA_DESTROY (priv_data, index) = destroy;
}

static void
free_priv_data (GstMiniObject * obj)
{
  guint i;
  gint priv_state = g_atomic_int_get ((gint *) & obj->priv_uint);
  PrivData *priv_data;

  if (priv_state != PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    if (priv_state == PRIV_DATA_STATE_LOCKED) {
      g_warning
          ("%s: object finalizing but has locked private data (object:%p)",
          G_STRFUNC, obj);
    } else if (priv_state == PRIV_DATA_STATE_ONE_PARENT) {
      g_warning
          ("%s: object finalizing but still has parent (object:%p, parent:%p)",
          G_STRFUNC, obj, obj->priv_pointer);
    }

    return;
  }

  priv_data = obj->priv_pointer;

  for (i = 0; i < priv_data->n_qdata; i++) {
    if (QDATA_QUARK (priv_data, i) == weak_ref_quark)
      QDATA_NOTIFY (priv_data, i) (QDATA_DATA (priv_data, i), obj);
    if (QDATA_DESTROY (priv_data, i))
      QDATA_DESTROY (priv_data, i) (QDATA_DATA (priv_data, i));
  }
  g_free (priv_data->qdata);

  if (priv_data->n_parents)
    g_warning ("%s: object finalizing but still has %d parents (object:%p)",
        G_STRFUNC, priv_data->n_parents, obj);
  g_free (priv_data->parents);

  g_free (priv_data);
}

/**
 * gst_mini_object_unref: (skip)
 * @mini_object: the mini-object
 *
 * Decreases the reference count of the mini-object, possibly freeing
 * the mini-object.
 */
void
gst_mini_object_unref (GstMiniObject * mini_object)
{
  gint old_refcount, new_refcount;

  g_return_if_fail (mini_object != NULL);
  g_return_if_fail (GST_MINI_OBJECT_REFCOUNT_VALUE (mini_object) > 0);

  old_refcount = g_atomic_int_add (&mini_object->refcount, -1);
  new_refcount = old_refcount - 1;

  g_return_if_fail (old_refcount > 0);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "%p unref %d->%d",
      mini_object, old_refcount, new_refcount);

  GST_TRACER_MINI_OBJECT_UNREFFED (mini_object, new_refcount);

  if (new_refcount == 0) {
    gboolean do_free;

    if (mini_object->dispose)
      do_free = mini_object->dispose (mini_object);
    else
      do_free = TRUE;

    /* 如果子类回收了对象（并返回了 FALSE），我们不再需要释放实例 */
    if (G_LIKELY (do_free)) {
      /* there should be no outstanding locks */
      g_return_if_fail ((g_atomic_int_get (&mini_object->lockstate) & LOCK_MASK)
          < 4);

      free_priv_data (mini_object);

      GST_TRACER_MINI_OBJECT_DESTROYED (mini_object);
      if (mini_object->free)
        mini_object->free (mini_object);
    }
  }
}

/**
 * gst_clear_mini_object: (skip)
 * @object_ptr: 指向 #GstMiniObject 引用的指针
 *
 * 清除对 #GstMiniObject 的引用。
 *
 * @object_ptr 不得为 %NULL。
 *
 * 如果引用为 %NULL，则此函数不执行任何操作。
 * 否则，使用 gst_mini_object_unref() 减少对象的引用计数，并将指针设置为 %NULL。
 *
 * 还包括一个宏，允许在不需要指针转换的情况下使用此函数。
 *
 * 自版本：1.16
 **/
#undef gst_clear_mini_object
void
gst_clear_mini_object (GstMiniObject ** object_ptr)
{
  g_clear_pointer (object_ptr, gst_mini_object_unref);
}

/**
 * gst_mini_object_replace:
 * @olddata: (inout) (transfer full) (nullable): 指向要被替换的迷你对象指针的指针
 * @newdata: (allow-none): 指向新迷你对象的指针
 *
 * 原子地修改一个指针，使其指向一个新的迷你对象。
 * 减少 @olddata 的引用计数，并增加 @newdata 的引用计数。
 *
 * @newdata 和 @olddata 指向的值都可以是 %NULL。
 *
 * 返回值：如果 @newdata 与 @olddata 不同，则返回 %TRUE
 */
gboolean
gst_mini_object_replace (GstMiniObject ** olddata, GstMiniObject * newdata)
{
  GstMiniObject *olddata_val;

  g_return_val_if_fail (olddata != NULL, FALSE);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "replace %p (%d) with %p (%d)",
      *olddata, *olddata ? (*olddata)->refcount : 0,
      newdata, newdata ? newdata->refcount : 0);

  olddata_val = (GstMiniObject *) g_atomic_pointer_get ((gpointer *) olddata);

  if (G_UNLIKELY (olddata_val == newdata))
    return FALSE;

  if (newdata)
    gst_mini_object_ref (newdata);

  while (G_UNLIKELY (!g_atomic_pointer_compare_and_exchange ((gpointer *)
              olddata, (gpointer) olddata_val, newdata))) {
    olddata_val = g_atomic_pointer_get ((gpointer *) olddata);
    if (G_UNLIKELY (olddata_val == newdata))
      break;
  }

  if (olddata_val)
    gst_mini_object_unref (olddata_val);

  return olddata_val != newdata;
}

/**
 * gst_mini_object_steal: (skip)
 * @olddata: (inout) (transfer full): 指向要被窃取的迷你对象指针的指针
 *
 * 用 %NULL 替换由 @olddata 指向的当前 #GstMiniObject 指针，并返回旧值。
 *
 * 返回值：(nullable)：位于 @olddata 处的 #GstMiniObject
 */
GstMiniObject *
gst_mini_object_steal (GstMiniObject ** olddata)
{
  GstMiniObject *olddata_val;

  g_return_val_if_fail (olddata != NULL, NULL);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "steal %p (%d)",
      *olddata, *olddata ? (*olddata)->refcount : 0);

  do {
    olddata_val = (GstMiniObject *) g_atomic_pointer_get ((gpointer *) olddata);
    if (olddata_val == NULL)
      break;
  } while (G_UNLIKELY (!g_atomic_pointer_compare_and_exchange ((gpointer *)
              olddata, (gpointer) olddata_val, NULL)));

  return olddata_val;
}

/**
 * gst_mini_object_take:
 * @olddata: (inout) (transfer full): 指向要被替换的迷你对象指针的指针
 * @newdata: 指向新迷你对象的指针
 *
 * 修改一个指针，使其指向一个新的迷你对象。这个修改是原子的。这个版本类似于 gst_mini_object_replace()，
 * 但不会增加 @newdata 的引用计数，因此会接管 @newdata 的所有权。
 *
 * @newdata 和 @olddata 指向的值都可以是 %NULL。
 *
 * 返回值：如果 @newdata 与 @olddata 不同，则返回 %TRUE
 */
gboolean
gst_mini_object_take (GstMiniObject ** olddata, GstMiniObject * newdata)
{
  GstMiniObject *olddata_val;

  g_return_val_if_fail (olddata != NULL, FALSE);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "take %p (%d) with %p (%d)",
      *olddata, *olddata ? (*olddata)->refcount : 0,
      newdata, newdata ? newdata->refcount : 0);

  do {
    olddata_val = (GstMiniObject *) g_atomic_pointer_get ((gpointer *) olddata);
    if (G_UNLIKELY (olddata_val == newdata))
      break;
  } while (G_UNLIKELY (!g_atomic_pointer_compare_and_exchange ((gpointer *)
              olddata, (gpointer) olddata_val, newdata)));

  if (olddata_val)
    gst_mini_object_unref (olddata_val);

  return olddata_val != newdata;
}

/**
 * gst_mini_object_weak_ref: (skip)
 * @object: 要弱引用的 #GstMiniObject
 * @notify: 在GstMiniObject被释放之前调用的回调函数
 * @data: 传递给回调函数的额外数据
 *
 * 在一个GstMiniObject对象上添加一个弱引用回调函数。弱引用用于在GstMiniObject对象被销毁时进行通知。
 * 它们被称为 "弱引用"，因为它们允许您安全地持有对GstMiniObject对象的指针，
 * 而不需要调用 gst_mini_object_ref()（gst_mini_object_ref() 添加强引用，
 * 即强制对象保持活动状态）。
 */
void
gst_mini_object_weak_ref (GstMiniObject * object,
    GstMiniObjectNotify notify, gpointer data)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (notify != NULL);
  g_return_if_fail (GST_MINI_OBJECT_REFCOUNT_VALUE (object) >= 1);

  G_LOCK (qdata_mutex);
  set_notify (object, -1, weak_ref_quark, notify, data, NULL);
  G_UNLOCK (qdata_mutex);
}

/**
 * gst_mini_object_weak_unref: (skip)
 * @object: 用于移除弱引用的 #GstMiniObject
 * @notify: 要搜索的回调函数
 * @data: 要搜索的数据
 *
 * 从GstMiniObject对象中移除一个弱引用回调。
 */
void
gst_mini_object_weak_unref (GstMiniObject * object,
    GstMiniObjectNotify notify, gpointer data)
{
  gint i;

  g_return_if_fail (object != NULL);
  g_return_if_fail (notify != NULL);

  G_LOCK (qdata_mutex);
  if ((i = find_notify (object, weak_ref_quark, TRUE, notify, data)) != -1) {
    remove_notify (object, i);
  } else {
    g_warning ("%s: couldn't find weak ref %p (object:%p data:%p)", G_STRFUNC,
        notify, object, data);
  }
  G_UNLOCK (qdata_mutex);
}

/**
 * gst_mini_object_set_qdata:
 * @object: 一个 #GstMiniObject
 * @quark: 一个 #GQuark，用于命名用户数据指针
 * @data: 一个不透明的用户数据指针
 * @destroy: 当 @data 需要被释放时调用的函数，以 @data 作为参数
 *
 * 这个函数在一个GstMiniObject上设置一个不透明的、命名的指针。
 * 名称通过一个 #GQuark 来指定（可以通过 g_quark_from_static_string() 等方式获取），
 * 而指针可以通过 gst_mini_object_get_qdata() 从 @object 中获取，直到 @object 被销毁。
 * 如果设置了之前已经设置的用户数据指针，它会覆盖（释放）旧的指针设置，使用 %NULL 作为指针
 * 实际上会移除存储的数据。
 *
 * 当 @object 被销毁，或者数据被通过具有相同 @quark 的 gst_mini_object_set_qdata() 调用覆盖时，
 * 可以指定 @destroy，它会以 @data 作为参数调用。
 */

void
gst_mini_object_set_qdata (GstMiniObject * object, GQuark quark,
    gpointer data, GDestroyNotify destroy)
{
  gint i;
  gpointer old_data = NULL;
  GDestroyNotify old_notify = NULL;

  g_return_if_fail (object != NULL);
  g_return_if_fail (quark > 0);

  G_LOCK (qdata_mutex);
  if ((i = find_notify (object, quark, FALSE, NULL, NULL)) != -1) {
    PrivData *priv_data = object->priv_pointer;

    old_data = QDATA_DATA (priv_data, i);
    old_notify = QDATA_DESTROY (priv_data, i);

    if (data == NULL)
      remove_notify (object, i);
  }
  if (data != NULL)
    set_notify (object, i, quark, NULL, data, destroy);
  G_UNLOCK (qdata_mutex);

  if (old_notify)
    old_notify (old_data);
}

/**
 * gst_mini_object_get_qdata:
 * @object: 用于从中获取存储的用户数据指针的 GstMiniObject
 * @quark: 一个 #GQuark，用于命名用户数据指针
 *
 * 这个函数用于获取通过 gst_mini_object_set_qdata() 存储的用户数据指针。
 *
 * 返回值：(transfer none) (nullable)：设置的用户数据指针，或者 %NULL
 */
gpointer
gst_mini_object_get_qdata (GstMiniObject * object, GQuark quark)
{
  guint i;
  gpointer result;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (quark > 0, NULL);

  G_LOCK (qdata_mutex);
  if ((i = find_notify (object, quark, FALSE, NULL, NULL)) != -1) {
    PrivData *priv_data = object->priv_pointer;
    result = QDATA_DATA (priv_data, i);
  } else {
    result = NULL;
  }
  G_UNLOCK (qdata_mutex);

  return result;
}

/**
 * gst_mini_object_steal_qdata:
 * @object: 用于从中获取存储的用户数据指针的 GstMiniObject
 * @quark: 一个 #GQuark，用于命名用户数据指针
 *
 * 这个函数用于获取通过 gst_mini_object_set_qdata() 存储的用户数据指针，
 * 并从 @object 中移除这些数据，同时不调用其 `destroy()` 函数（如果设置了的话）。
 *
 * 返回值：(transfer full) (nullable)：设置的用户数据指针，或者 %NULL
 */
gpointer
gst_mini_object_steal_qdata (GstMiniObject * object, GQuark quark)
{
  guint i;
  gpointer result;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (quark > 0, NULL);

  G_LOCK (qdata_mutex);
  if ((i = find_notify (object, quark, FALSE, NULL, NULL)) != -1) {
    PrivData *priv_data = object->priv_pointer;
    result = QDATA_DATA (priv_data, i);
    remove_notify (object, i);
  } else {
    result = NULL;
  }
  G_UNLOCK (qdata_mutex);

  return result;
}

/**
 * gst_mini_object_add_parent:
 * @object: 一个 #GstMiniObject
 * @parent: 一个父 #GstMiniObject
 *
 * 这个函数将 @parent 添加为 @object 的一个父对象。拥有一个或多个父对象会影响
 * @object 的可写性：如果一个 @parent 不可写，那么 @object 也不可写，
 * 无论其引用计数是多少。@object 只有在所有父对象都是可写的，并且其自身的引用计数
 * 正好为1时，才是可写的。
 *
 * 注意：这个函数不会获取 @parent 的所有权，也不会增加额外的引用计数。
 * 调用者有责任在之后的某个时间点移除父对象。
 *
 * 自版本 1.16 起可用
 */

void
gst_mini_object_add_parent (GstMiniObject * object, GstMiniObject * parent)
{
  gint priv_state;

  g_return_if_fail (object != NULL);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "adding parent %p to object %p", parent,
      object);

  priv_state = lock_priv_pointer (object);
  
  /*如果已经有一个父对象，现在需要分配整个（完整）结构体*/
  if (priv_state == PRIV_DATA_STATE_ONE_PARENT) {
    /* Unlock again */
    g_atomic_int_set ((gint *) & object->priv_uint, priv_state);

    ensure_priv_data (object);
    priv_state = PRIV_DATA_STATE_PARENTS_OR_QDATA;
  }

  /* 现在我们要么需要将新的父对象添加到完整的结构体中，要么将我们唯一的一个父对象添加到指针字段中 */
  if (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    PrivData *priv_data = object->priv_pointer;

    /* Lock parents */
    while (!g_atomic_int_compare_and_exchange (&priv_data->parent_lock, 0, 1));

    if (priv_data->n_parents >= priv_data->n_parents_len) {
      priv_data->n_parents_len *= 2;
      if (priv_data->n_parents_len == 0)
        priv_data->n_parents_len = 16;

      priv_data->parents =
          g_realloc (priv_data->parents,
          priv_data->n_parents_len * sizeof (GstMiniObject *));
    }
    priv_data->parents[priv_data->n_parents] = parent;
    priv_data->n_parents++;

    /* Unlock again */
    g_atomic_int_set (&priv_data->parent_lock, 0);
  } else if (priv_state == PRIV_DATA_STATE_NO_PARENT) {
    object->priv_pointer = parent;

    /* Unlock again */
    g_atomic_int_set ((gint *) & object->priv_uint, PRIV_DATA_STATE_ONE_PARENT);
  } else {
    g_assert_not_reached ();
  }
}

/**
 * gst_mini_object_remove_parent:
 * @object: a #GstMiniObject
 * @parent: a parent #GstMiniObject
 *
 * This removes @parent as a parent for @object. See
 * gst_mini_object_add_parent().
 *
 * Since: 1.16
 */
void
gst_mini_object_remove_parent (GstMiniObject * object, GstMiniObject * parent)
{
  gint priv_state;

  g_return_if_fail (object != NULL);

  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "removing parent %p from object %p",
      parent, object);

  priv_state = lock_priv_pointer (object);

  /* 现在我们必须将新的父元素添加到完整的结构中，或者将我们唯一的父元素添加到指针字段中 */
  if (priv_state == PRIV_DATA_STATE_PARENTS_OR_QDATA) {
    PrivData *priv_data = object->priv_pointer;
    guint i;

    /* Lock parents */
    while (!g_atomic_int_compare_and_exchange (&priv_data->parent_lock, 0, 1));

    /* 在parent数组里面找到要要删除的那个parent */
    for (i = 0; i < priv_data->n_parents; i++)
      if (parent == priv_data->parents[i])
        break;

    if (i != priv_data->n_parents) {
      priv_data->n_parents--;
      if (priv_data->n_parents != i)
        priv_data->parents[i] = priv_data->parents[priv_data->n_parents];
    } else {
      g_warning ("%s: couldn't find parent %p (object:%p)", G_STRFUNC,
          object, parent);
    }

    /* Unlock again */
    g_atomic_int_set (&priv_data->parent_lock, 0);
  } else if (priv_state == PRIV_DATA_STATE_ONE_PARENT) {
    if (object->priv_pointer != parent) {
      g_warning ("%s: couldn't find parent %p (object:%p)", G_STRFUNC,
          object, parent);
      /* Unlock again */
      g_atomic_int_set ((gint *) & object->priv_uint, priv_state);
    } else {
      object->priv_pointer = NULL;
      /* Unlock again */
      g_atomic_int_set ((gint *) & object->priv_uint,
          PRIV_DATA_STATE_NO_PARENT);
    }
  } else {
    /* Unlock again */
    g_atomic_int_set ((gint *) & object->priv_uint, PRIV_DATA_STATE_NO_PARENT);
  }
}
