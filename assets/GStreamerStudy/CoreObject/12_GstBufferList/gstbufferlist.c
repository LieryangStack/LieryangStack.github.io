#define GST_DISABLE_MINIOBJECT_INLINE_FUNCTIONS
#include "gst_private.h"

#include "gstbuffer.h"
#include "gstbufferlist.h"
#include "gstutils.h"

#define GST_CAT_DEFAULT GST_CAT_BUFFER_LIST

#define GST_BUFFER_LIST_IS_USING_DYNAMIC_ARRAY(list) \
    ((list)->buffers != &(list)->arr[0])

/**
 * GstBufferList:
 *
 * Opaque list of grouped buffers.
 */
struct _GstBufferList
{
  GstMiniObject mini_object;

  GstBuffer **buffers;
  guint n_buffers; /* 目前列表存储有多少个buffer */
  guint n_allocated; /* 列表可以存储多少个buffer（分配了多少个buffer指针存储） */

  gsize slice_size; /* sizeof(GstBufferList) + ( n_allocated - 1 ) * sizeof(void *) */

  /* 虽然这里的GstBuffer指针数组只能存储一个，但是初始化函数分配了sizeof（GstBufferList）+ 存储指针的连续空间 
   * 意味着 arr[2] arr[3] .... arr[n_allocated] 都有效
   * @note: sizeof（GstBufferList） = 96，这个结构体的最后一个成员 arr是占用 88字节，所以不会出现结构体内存对齐问题
   */
  GstBuffer *arr[1];
};

GType _gst_buffer_list_type = 0;

GST_DEFINE_MINI_OBJECT_TYPE (GstBufferList, gst_buffer_list);

void
_priv_gst_buffer_list_initialize (void)
{
  _gst_buffer_list_type = gst_buffer_list_get_type ();
}

static GstBufferList *
_gst_buffer_list_copy (GstBufferList * list)
{
  GstBufferList *copy;
  guint i, len;

  len = list->n_buffers;
  copy = gst_buffer_list_new_sized (list->n_allocated);

  /* add and ref all buffers in the array */
  for (i = 0; i < len; i++) {
    copy->buffers[i] = gst_buffer_ref (list->buffers[i]);
    gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (copy->buffers[i]),
        GST_MINI_OBJECT_CAST (copy));
  }

  copy->n_buffers = len;

  return copy;
}

static void
_gst_buffer_list_free (GstBufferList * list)
{
  guint i, len;
  gsize slice_size;

  GST_LOG ("free %p", list);

  /* unrefs all buffers too */
  len = list->n_buffers;
  for (i = 0; i < len; i++) {
    gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (list->buffers[i]),
        GST_MINI_OBJECT_CAST (list));
    gst_buffer_unref (list->buffers[i]);
  }

  if (GST_BUFFER_LIST_IS_USING_DYNAMIC_ARRAY (list))
    g_free (list->buffers);

  slice_size = list->slice_size;

#ifdef USE_POISONING
  memset (list, 0xff, slice_size);
#endif

  g_slice_free1 (slice_size, list);
}

static void
gst_buffer_list_init (GstBufferList * list, guint n_allocated, gsize slice_size)
{
  gst_mini_object_init (GST_MINI_OBJECT_CAST (list), 0, _gst_buffer_list_type,
      (GstMiniObjectCopyFunction) _gst_buffer_list_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_buffer_list_free);

  list->buffers = &list->arr[0];
  list->n_buffers = 0;
  list->n_allocated = n_allocated;
  list->slice_size = slice_size;

  GST_LOG ("init %p", list);
}


/* 创建一个新的空的#GstBufferList。列表将有@size空间预分配以便可以避免内存重新分配。 */
GstBufferList *
gst_buffer_list_new_sized (guint size)
{
  GstBufferList *list;
  gsize slice_size;
  guint n_allocated;

  if (size == 0)
    size = 1;

  /* 向上舍入到最接近16的倍数 */
  n_allocated = GST_ROUND_UP_16 (size);

  /* 分配 GstBufferList 和 存储Buffer地址的指针 内存空间 */
  slice_size = sizeof (GstBufferList) + (n_allocated - 1) * sizeof (gpointer);

  list = g_slice_alloc0 (slice_size);

  GST_LOG ("new %p", list);

  gst_buffer_list_init (list, n_allocated, slice_size);

  return list;
}


/**
 * @brief: 创建一个new，empty #GstBufferList
 * @call: gst_buffer_list_new
*/
GstBufferList *
gst_buffer_list_new (void)
{
  return gst_buffer_list_new_sized (8);
}


/* GstBufferList中有多少个Buffer */
guint
gst_buffer_list_length (GstBufferList * list)
{
  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), 0);

  return list->n_buffers;
}

/**
 * @param unref_old: 是否解引用要被删除的buffer
 * @brief: 删除从@idx开始，@length个buffer
*/
static inline void
gst_buffer_list_remove_range_internal (GstBufferList * list, guint idx,
    guint length, gboolean unref_old)
{
  guint i;

  if (unref_old) {
    for (i = idx; i < idx + length; ++i) {
      gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (list->buffers[i]),
          GST_MINI_OBJECT_CAST (list));
      gst_buffer_unref (list->buffers[i]);
    }
  }

  if (idx + length != list->n_buffers) {
    memmove (&list->buffers[idx], &list->buffers[idx + length],
        (list->n_buffers - (idx + length)) * sizeof (void *));
  }

  list->n_buffers -= length;
}


/**
 * @name: gst_buffer_list_foreach
 * @param list: a #GstBufferList
 * @param func: a #GstBufferListFunc to call
 * @param user_data: user data passed to @func
 * @note: 如果 @func 函数返回FALSE，停止遍历
 * @brief: @func可以修改传递的缓冲区指针或其内容
*/
gboolean
gst_buffer_list_foreach (GstBufferList * list, GstBufferListFunc func,
    gpointer user_data)
{
  guint i, len;
  gboolean ret = TRUE;
  gboolean list_was_writable, first_warning = TRUE;

  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), FALSE);
  g_return_val_if_fail (func != NULL, FALSE);

  list_was_writable = gst_buffer_list_is_writable (list);

  len = list->n_buffers;
  for (i = 0; i < len;) {
    GstBuffer *buf, *buf_ret;
    gboolean was_writable;

    buf = buf_ret = list->buffers[i];

    /**
     * 如果缓冲区是可写的，我们移除缓冲区parent，也就是@list，以便运行@func函数能够销毁buffer。
     * 我们等到buffer返回，我们再次添加@List作为parent。
     * 
     * 不可写缓冲区只会得到一个ref引用，因为它们本来就不可写，并且通过将我们自己移除作为父项，它们可能会变得可写。
    */
    was_writable = list_was_writable && gst_buffer_is_writable (buf);

    if (was_writable)
      gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (buf),
          GST_MINI_OBJECT_CAST (list));
    else
      gst_buffer_ref (buf);

    ret = func (&buf_ret, i, user_data);

    /* 检查返回的buffer和传入的buffer是否发生变化 */
    if (buf != buf_ret) {
      /**
       * 如果列表本来是不可写的，但回调函数实际上改变了我们的缓冲区，那么它本来是不允许这样做的
       * 幸运的是，在这种情况下，我们仍然有对旧缓冲区的引用，只是不修改列表，释放新缓冲区（如果有的话），并发出警告。
      */
      if (!list_was_writable) {
        if (first_warning) {
          g_critical
              ("gst_buffer_list_foreach: non-writable list %p was changed from callback",
              list);
          first_warning = FALSE;
        }
        if (buf_ret)
          gst_buffer_unref (buf_ret);
      } else if (buf_ret == NULL) { /* 返回的buffer地址为空，也就是被该buffer删除了 */
        gst_buffer_list_remove_range_internal (list, i, 1, !was_writable);
        --len;
      } else {
        if (!was_writable) { /* 如果不可写，我们移除parent */
          gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (buf),
              GST_MINI_OBJECT_CAST (list));
          gst_buffer_unref (buf);
        }

        list->buffers[i] = buf_ret;
        gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (buf_ret),
            GST_MINI_OBJECT_CAST (list));
      }
    } else {  /* 返回的buffer 和 传入的 buffer 地址没有发生变化 */
      if (was_writable)
        gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (buf),
            GST_MINI_OBJECT_CAST (list));
      else
        gst_buffer_unref (buf);
    }

    if (!ret)
      break;

    /* If the buffer was not removed by func go to the next buffer */
    if (buf_ret != NULL)
      i++;
  }
  return ret;
}


/**
 * @brief: 获取 @idx 处的缓冲区。
 * @note: 您必须确保 @idx 不超过可用缓冲区的数量。
*/
GstBuffer *
gst_buffer_list_get (GstBufferList * list, guint idx)
{
  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), NULL);
  g_return_val_if_fail (idx < list->n_buffers, NULL);

  return list->buffers[idx];
}


/**
 * @brief: 获取 @idx 处的缓冲区，并确保它是一个可写的缓冲区。
 * @note: 您必须确保 @idx 不超过可用缓冲区的数量。
*/
GstBuffer *
gst_buffer_list_get_writable (GstBufferList * list, guint idx)
{
  GstBuffer *new_buf;

  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), NULL);
  g_return_val_if_fail (gst_buffer_list_is_writable (list), NULL);
  g_return_val_if_fail (idx < list->n_buffers, NULL);

  /* We have to implement this manually here to correctly add/remove the
   * parent */
  if (gst_buffer_is_writable (list->buffers[idx]))
    return list->buffers[idx];

  gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (list->buffers[idx]),
      GST_MINI_OBJECT_CAST (list));
  new_buf = gst_buffer_copy (list->buffers[idx]);
  gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (new_buf),
      GST_MINI_OBJECT_CAST (list));
  gst_buffer_unref (list->buffers[idx]);
  list->buffers[idx] = new_buf;

  return new_buf;
}


/**
 * @brief: 在 @list 中的 @idx 处插入 @buffer。其他缓冲区将被移动，为这个新缓冲区腾出空间。
 * @note: @idx 的值为 -1 时将在末尾追加缓冲区。
*/
void
gst_buffer_list_insert (GstBufferList * list, gint idx, GstBuffer * buffer)
{
  guint want_alloc;

  g_return_if_fail (GST_IS_BUFFER_LIST (list));
  g_return_if_fail (buffer != NULL);
  g_return_if_fail (gst_buffer_list_is_writable (list));

  if (idx == -1 && list->n_buffers < list->n_allocated) {
    gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (buffer),
        GST_MINI_OBJECT_CAST (list));
    list->buffers[list->n_buffers++] = buffer;
    return;
  }

  if (idx == -1 || idx > list->n_buffers)
    idx = list->n_buffers;

  want_alloc = list->n_buffers + 1;

  if (want_alloc > list->n_allocated) {
    if (G_UNLIKELY (list->n_allocated > (G_MAXUINT / 2)))
      g_error ("Growing GstBufferList would result in overflow");

    want_alloc = MAX (GST_ROUND_UP_16 (want_alloc), list->n_allocated * 2);

    if (GST_BUFFER_LIST_IS_USING_DYNAMIC_ARRAY (list)) {
      list->buffers = g_renew (GstBuffer *, list->buffers, want_alloc);
    } else {
      list->buffers = g_new0 (GstBuffer *, want_alloc);
      memcpy (list->buffers, &list->arr[0], list->n_buffers * sizeof (void *));
      GST_CAT_LOG (GST_CAT_PERFORMANCE, "exceeding pre-alloced array");
    }

    list->n_allocated = want_alloc;
  }

  if (idx < list->n_buffers) {
    memmove (&list->buffers[idx + 1], &list->buffers[idx],
        (list->n_buffers - idx) * sizeof (void *));
  }

  ++list->n_buffers;
  list->buffers[idx] = buffer;
  gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (buffer),
      GST_MINI_OBJECT_CAST (list));
}

/**
 * @name: gst_buffer_list_remove:
 * @param list: a #GstBufferList
 * @param idx: the index
 * @param length: the amount to remove
 * @breif: 从 @list 中的 @idx 开始移除 @length 个缓冲区。随后的缓冲区会被移动以填补空位。
 */
void
gst_buffer_list_remove (GstBufferList * list, guint idx, guint length)
{
  g_return_if_fail (GST_IS_BUFFER_LIST (list));
  g_return_if_fail (idx < list->n_buffers);
  g_return_if_fail (idx + length <= list->n_buffers);
  g_return_if_fail (gst_buffer_list_is_writable (list));

  gst_buffer_list_remove_range_internal (list, idx, length, TRUE);
}

/**
 * gst_buffer_list_copy_deep:
 * @list: a #GstBufferList
 *
 * Creates a copy of the given buffer list. This will make a newly allocated
 * copy of the buffers that the source buffer list contains.
 *
 * Returns: (transfer full): a new copy of @list.
 *
 * Since: 1.6
 */
GstBufferList *
gst_buffer_list_copy_deep (const GstBufferList * list)
{
  guint i, len;
  GstBufferList *result = NULL;

  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), NULL);

  result = gst_buffer_list_new ();

  len = list->n_buffers;
  for (i = 0; i < len; i++) {
    GstBuffer *old = list->buffers[i];
    GstBuffer *new = gst_buffer_copy_deep (old);

    if (G_LIKELY (new)) {
      gst_buffer_list_insert (result, i, new);
    } else {
      g_warning
          ("Failed to deep copy buffer %p while deep "
          "copying buffer list %p. Buffer list copy "
          "will be incomplete", old, list);
    }
  }

  return result;
}

/**
 * gst_buffer_list_calculate_size:
 * @list: a #GstBufferList
 *
 * Calculates the size of the data contained in @list by adding the
 * size of all buffers.
 *
 * Returns: the size of the data contained in @list in bytes.
 *
 * Since: 1.14
 */
gsize
gst_buffer_list_calculate_size (GstBufferList * list)
{
  GstBuffer **buffers;
  gsize size = 0;
  guint i, n;

  g_return_val_if_fail (GST_IS_BUFFER_LIST (list), 0);

  n = list->n_buffers;
  buffers = list->buffers;

  for (i = 0; i < n; ++i)
    size += gst_buffer_get_size (buffers[i]);

  return size;
}

/**
 * gst_buffer_list_ref: (skip)
 * @list: a #GstBufferList
 *
 * Increases the refcount of the given buffer list by one.
 *
 * Note that the refcount affects the writability of @list and its data, see
 * gst_buffer_list_make_writable(). It is important to note that keeping
 * additional references to GstBufferList instances can potentially increase
 * the number of memcpy operations in a pipeline.
 *
 * Returns: (transfer full): @list
 */
GstBufferList *
gst_buffer_list_ref (GstBufferList * list)
{
  return
      GST_BUFFER_LIST_CAST (gst_mini_object_ref (GST_MINI_OBJECT_CAST (list)));
}

/**
 * gst_buffer_list_unref: (skip)
 * @list: (transfer full): a #GstBufferList
 *
 * Decreases the refcount of the buffer list. If the refcount reaches 0, the
 * buffer list will be freed.
 */
void
gst_buffer_list_unref (GstBufferList * list)
{
  gst_mini_object_unref (GST_MINI_OBJECT_CAST (list));
}

/**
 * gst_clear_buffer_list: (skip)
 * @list_ptr: a pointer to a #GstBufferList reference
 *
 * Clears a reference to a #GstBufferList.
 *
 * @list_ptr must not be %NULL.
 *
 * If the reference is %NULL then this function does nothing. Otherwise, the
 * reference count of the list is decreased and the pointer is set to %NULL.
 *
 * Since: 1.16
 */
void
gst_clear_buffer_list (GstBufferList ** list_ptr)
{
  gst_clear_mini_object ((GstMiniObject **) list_ptr);
}

/**
 * gst_buffer_list_copy: (skip)
 * @list: a #GstBufferList
 *
 * Creates a shallow copy of the given buffer list. This will make a newly
 * allocated copy of the source list with copies of buffer pointers. The
 * refcount of buffers pointed to will be increased by one.
 *
 * Returns: (transfer full): a new copy of @list.
 */
GstBufferList *
gst_buffer_list_copy (const GstBufferList * list)
{
  return
      GST_BUFFER_LIST_CAST (gst_mini_object_copy (GST_MINI_OBJECT_CONST_CAST
          (list)));
}

/**
 * gst_buffer_list_replace:
 * @old_list: (inout) (transfer full) (nullable): pointer to a pointer to a
 *     #GstBufferList to be replaced.
 * @new_list: (transfer none) (allow-none): pointer to a #GstBufferList that
 *     will replace the buffer list pointed to by @old_list.
 *
 * Modifies a pointer to a #GstBufferList to point to a different
 * #GstBufferList. The modification is done atomically (so this is useful for
 * ensuring thread safety in some cases), and the reference counts are updated
 * appropriately (the old buffer list is unreffed, the new is reffed).
 *
 * Either @new_list or the #GstBufferList pointed to by @old_list may be %NULL.
 *
 * Returns: %TRUE if @new_list was different from @old_list
 *
 * Since: 1.16
 */
gboolean
gst_buffer_list_replace (GstBufferList ** old_list, GstBufferList * new_list)
{
  return gst_mini_object_replace ((GstMiniObject **) old_list,
      (GstMiniObject *) new_list);
}

/**
 * gst_buffer_list_take:
 * @old_list: (inout) (transfer full): pointer to a pointer to a #GstBufferList
 *     to be replaced.
 * @new_list: (transfer full) (allow-none): pointer to a #GstBufferList
 *     that will replace the bufferlist pointed to by @old_list.
 *
 * Modifies a pointer to a #GstBufferList to point to a different
 * #GstBufferList. This function is similar to gst_buffer_list_replace() except
 * that it takes ownership of @new_list.
 *
 * Returns: %TRUE if @new_list was different from @old_list
 *
 * Since: 1.16
 */
gboolean
gst_buffer_list_take (GstBufferList ** old_list, GstBufferList * new_list)
{
  return gst_mini_object_take ((GstMiniObject **) old_list,
      (GstMiniObject *) new_list);
}