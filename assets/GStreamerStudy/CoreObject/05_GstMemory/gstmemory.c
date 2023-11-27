#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define GST_DISABLE_MINIOBJECT_INLINE_FUNCTIONS
#include "gst_private.h"
#include "gstmemory.h"

/* lieryang add */
#include <gst/gst.h>

GType _gst_memory_type = 0;
GST_DEFINE_MINI_OBJECT_TYPE (GstMemory, gst_memory);

/**
 * 实际调用的是分配器实例中的内存拷贝函数
*/
static GstMemory *
_gst_memory_copy (GstMemory * mem)
{
  GST_CAT_DEBUG (GST_CAT_MEMORY, "copy memory %p", mem);
  return gst_memory_copy (mem, 0, -1);
}

/**
 * 实际调用的是分配器类中的内存释放函数
*/
static void
_gst_memory_free (GstMemory * mem)
{
  GstAllocator *allocator;

  GST_CAT_DEBUG (GST_CAT_MEMORY, "free memory %p", mem);

  if (mem->parent) {
    gst_memory_unlock (mem->parent, GST_LOCK_FLAG_EXCLUSIVE);
    gst_memory_unref (mem->parent);
  }

  allocator = mem->allocator;

  gst_allocator_free (allocator, mem);

  gst_object_unref (allocator);
}

/**
 * gst_memory_init: (skip)
 * @breif: 使用给定参数初始化新分配的 @mem。这个函数会调用 gst_mini_object_init() 并使用默认的内存参数。
 * @mem: 一个 #GstMemory
 * @flags: #GstMemoryFlags
 * @allocator: #GstAllocator
 * @parent: @mem 的父内存
 * @maxsize: 内存的总大小
 * @align: 内存的对齐方式
 * @offset: 内存中的偏移量
 * @size: 内存中有效数据的大小
 * 
 * @note: 初始化GstMemory对象GST_MINI_OBJECT_FLAG_LOCKABLE
 */
void
gst_memory_init (GstMemory * mem, GstMemoryFlags flags,
    GstAllocator * allocator, GstMemory * parent, gsize maxsize, gsize align,
    gsize offset, gsize size)
{

  gst_mini_object_init (GST_MINI_OBJECT_CAST (mem),
      flags | GST_MINI_OBJECT_FLAG_LOCKABLE, GST_TYPE_MEMORY,
      (GstMiniObjectCopyFunction) _gst_memory_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_memory_free);

  mem->allocator = gst_object_ref (allocator);
  if (parent) {
    /* FIXME 2.0: this can fail if the memory is already write locked */
    gst_memory_lock (parent, GST_LOCK_FLAG_EXCLUSIVE);
    gst_memory_ref (parent);
  }
  mem->parent = parent;
  mem->maxsize = maxsize;
  mem->align = align;
  mem->offset = offset;
  mem->size = size;

  GST_CAT_DEBUG (GST_CAT_MEMORY, "new memory %p, maxsize:%" G_GSIZE_FORMAT
      " offset:%" G_GSIZE_FORMAT " size:%" G_GSIZE_FORMAT, mem, maxsize,
      offset, size);
}

/**
 * gst_memory_is_type:
 * @brief: 传入的@mem_type和分配器名称比较。检查 @mem 是否是使用分配给 @mem_type 的分配器分配的。
 * @mem: 一个 #GstMemory
 * @mem_type: 一种内存类型
 * @return 返回值：如果 @mem 是从分配给 @mem_type 的分配器分配的，则返回 %TRUE。
 *
 * 自版本 1.2 起。
 */
gboolean
gst_memory_is_type (GstMemory * mem, const gchar * mem_type)
{
  g_return_val_if_fail (mem != NULL, FALSE);
  g_return_val_if_fail (mem->allocator != NULL, FALSE);
  g_return_val_if_fail (mem_type != NULL, FALSE);

  return (g_strcmp0 (mem->allocator->mem_type, mem_type) == 0);
}

/**
 * gst_memory_get_sizes:
 * @brief: 获取GstMemory结构体中存储的offset和maxsize到指针参数中
 * @mem: 一个 #GstMemory
 * @offset: (out) (allow-none): 指向偏移量的指针
 * @maxsize: (out) (allow-none): 指向最大大小的指针
 *
 * 获取 @mem 的当前 @size、@offset 和 @maxsize。
 *
 * 返回值：@mem 的当前大小
 */
gsize
gst_memory_get_sizes (GstMemory * mem, gsize * offset, gsize * maxsize)
{
  g_return_val_if_fail (mem != NULL, 0);

  if (offset)
    *offset = mem->offset;
  if (maxsize)
    *maxsize = mem->maxsize;

  return mem->size;
}

/**
 * gst_memory_resize:
 * @mem: 一个 #GstMemory
 * @offset: 一个新的偏移量
 * @size: 一个新的大小
 *
 * 调整内存区域的大小。@mem 应该是可写的，并且 offset + size 应该小于 @mem 的最大大小。
 *
 * 当偏移量或填充增加时，#GST_MEMORY_FLAG_ZERO_PREFIXED 和 #GST_MEMORY_FLAG_ZERO_PADDED 将分别被清除。
 */
void
gst_memory_resize (GstMemory * mem, gssize offset, gsize size)
{
  g_return_if_fail (mem != NULL);
  g_return_if_fail (gst_memory_is_writable (mem));
  g_return_if_fail (offset >= 0 || mem->offset >= -offset);
  g_return_if_fail (size + mem->offset + offset <= mem->maxsize);

  /* if we increase the prefix, we can't guarantee it is still 0 filled */
  if ((offset > 0) && GST_MEMORY_IS_ZERO_PREFIXED (mem))
    GST_MEMORY_FLAG_UNSET (mem, GST_MEMORY_FLAG_ZERO_PREFIXED);

  /* if we increase the padding, we can't guarantee it is still 0 filled */
  if ((offset + size < mem->size) && GST_MEMORY_IS_ZERO_PADDED (mem))
    GST_MEMORY_FLAG_UNSET (mem, GST_MEMORY_FLAG_ZERO_PADDED);

  mem->offset += offset;
  mem->size = size;
}

/**
 * gst_memory_make_mapped:
 * @mem: (transfer full): 一个 #GstMemory
 * @info: (out caller-allocates): 用于信息的指针
 * @flags: 映射标志
 *
 * 创建一个用 @flags 映射的 #GstMemory 对象。如果 @mem 可以用 @flags 映射，这个函数直接返回映射的 @mem。否则，返回 @mem 的一个映射副本。
 *
 * 这个函数接管了旧的 @mem 并返回一个新的 #GstMemory 的引用。
 *
 * 返回值：(transfer full) (nullable): 一个用 @flags 映射的 #GstMemory 对象，或当映射不可能时返回 %NULL。
 */
GstMemory *
gst_memory_make_mapped (GstMemory * mem, GstMapInfo * info, GstMapFlags flags)
{
  GstMemory *result;

  if (gst_memory_map (mem, info, flags)) {
    result = mem;
  } else {
    result = gst_memory_copy (mem, 0, -1);
    gst_memory_unref (mem);

    if (result == NULL)
      goto cannot_copy;

    if (!gst_memory_map (result, info, flags))
      goto cannot_map;
  }
  return result;

  /* ERRORS */
cannot_copy:
  {
    GST_CAT_DEBUG (GST_CAT_MEMORY, "cannot copy memory %p", mem);
    return NULL;
  }
cannot_map:
  {
    GST_CAT_DEBUG (GST_CAT_MEMORY, "cannot map memory %p with flags %d", mem,
        flags);
    gst_memory_unref (result);
    return NULL;
  }
}

/**
 * gst_memory_map:
 * @brief: 因为GstMemory结构体中并不包含用户存储数据的data，通常用户会根据实际需要创建一个继承于GstMemory的结构体，存储data。
 *         使用用户继承于的GstAllocator的分配器，分配GstMemory内存。
 *         map函数就是把data经过一些处理（偏移等操作），赋值到GstMapInfo结构体中。
 * @mem: 一个 #GstMemory
 * @info: (out caller-allocates): 用于信息的指针
 * @flags: 映射标志
 *
 * 根据 @flags，用 @mem 中可访问的内存的指针和大小填充 @info。
 *
 * 出于各种原因，此函数可能返回 %FALSE：
 * - 由 @mem 支持的内存不能用给定的 @flags 访问。
 * - 内存已经以不同的映射方式被映射过。
 *
 * 只要 @mem 有效，并且直到调用 gst_memory_unmap()，@info 及其内容就保持有效。
 *
 * 对于每个 gst_memory_map() 调用，应该执行相应的 gst_memory_unmap() 调用。
 *
 * 返回值：如果映射操作成功，则返回 %TRUE。
 */
gboolean
gst_memory_map (GstMemory * mem, GstMapInfo * info, GstMapFlags flags)
{
  g_return_val_if_fail (mem != NULL, FALSE);
  g_return_val_if_fail (info != NULL, FALSE);

  if (!gst_memory_lock (mem, (GstLockFlags) flags))
    goto lock_failed;

  info->flags = flags;
  info->memory = mem;
  info->size = mem->size;
  info->maxsize = mem->maxsize - mem->offset;

  /* 关键部分 info->data才是存储用户数据的指针 */
  if (mem->allocator->mem_map_full)
    info->data = mem->allocator->mem_map_full (mem, info, mem->maxsize);
  else
    info->data = mem->allocator->mem_map (mem, mem->maxsize, flags);

  if (G_UNLIKELY (info->data == NULL))
    goto error;

  info->data = info->data + mem->offset;

  return TRUE;

  /* ERRORS */
lock_failed:
  {
    GST_CAT_DEBUG (GST_CAT_MEMORY, "mem %p: lock %d failed", mem, flags);
    memset (info, 0, sizeof (GstMapInfo));
    return FALSE;
  }
error:
  {
    /* something went wrong, restore the original state again
     * it is up to the subclass to log an error if needed. */
    GST_CAT_INFO (GST_CAT_MEMORY, "mem %p: subclass map failed", mem);
    gst_memory_unlock (mem, (GstLockFlags) flags);
    memset (info, 0, sizeof (GstMapInfo));
    return FALSE;
  }
}

/**
 * gst_memory_unmap:
 * @mem: 一个 #GstMemory
 * @info: 一个 #GstMapInfo
 *
 * 释放通过 gst_memory_map() 获取的内存。
 */
void
gst_memory_unmap (GstMemory * mem, GstMapInfo * info)
{
  g_return_if_fail (mem != NULL);
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->memory == mem);

  if (mem->allocator->mem_unmap_full)
    mem->allocator->mem_unmap_full (mem, info);
  else
    mem->allocator->mem_unmap (mem);
  gst_memory_unlock (mem, (GstLockFlags) info->flags);
}

/**
 * gst_memory_copy:
 * @mem: 一个 #GstMemory
 * @offset: 要复制的偏移量
 * @size: 要复制的大小，或者 -1 表示复制到内存区域的末尾
 *
 * 从 @mem 的 @offset 开始返回 @size 字节的副本。这个副本保证是可写的。@size 可以设置为 -1 来返回从 @offset 到内存区域末尾的副本。
 *
 * 返回值：(transfer full) (nullable): 如果复制成功，返回 @mem 的新副本，否则返回 %NULL。
 */
GstMemory *
gst_memory_copy (GstMemory * mem, gssize offset, gssize size)
{
  GstMemory *copy;

  g_return_val_if_fail (mem != NULL, NULL);

  copy = mem->allocator->mem_copy (mem, offset, size);

  return copy;
}

/**
 * gst_memory_share:
 * @mem: 一个 #GstMemory
 * @offset: 要共享的偏移量
 * @size: 要共享的大小，或者 -1 表示共享到内存区域的末尾
 *
 * 从 @mem 的 @offset 开始返回 @size 字节的共享副本。不执行内存复制，内存区域简单地被共享。结果保证是不可写的。
 * @size 可以设置为 -1 来返回从 @offset 到内存区域末尾的共享副本。
 *
 * 返回值：一个新的 #GstMemory。
 */
GstMemory *
gst_memory_share (GstMemory * mem, gssize offset, gssize size)
{
  GstMemory *shared;

  g_return_val_if_fail (mem != NULL, NULL);
  g_return_val_if_fail (!GST_MEMORY_FLAG_IS_SET (mem, GST_MEMORY_FLAG_NO_SHARE),
      NULL);

  /* 是否可以独占地锁定内存 */
  /* 为了保持向后兼容性，不要求子类自己锁定内存并在它们的 mem_share 实现中传播可能的失败 */
  /* FIXME 2.0：移除并修复 gst_memory_init() 和/或所有内存子类以传播这种失败情况 */
  if (!gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE))
    return NULL;

  /* 两次独有锁的时候，其他用户不能再上写锁 */
  if (!gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE)) {
    gst_memory_unlock (mem, GST_LOCK_FLAG_EXCLUSIVE);
    return NULL;
  }

  shared = mem->allocator->mem_share (mem, offset, size);

  /* unlocking before calling the subclass would be racy */
  gst_memory_unlock (mem, GST_LOCK_FLAG_EXCLUSIVE);
  gst_memory_unlock (mem, GST_LOCK_FLAG_EXCLUSIVE);

  return shared;
}

/**
 * gst_memory_is_span:
 * @mem1: 一个 #GstMemory
 * @mem2: 一个 #GstMemory
 * @offset: (out): 结果偏移量的指针
 *
 * 检查 @mem1 和 mem2 是否与一个共同的父内存对象共享内存，并且内存是连续的。
 *
 * 如果是这种情况，通过在父对象上执行 gst_memory_share()，可以有效地合并 @mem1 和 @mem2 的内存，并从返回的 @offset 开始。
 *
 * 返回值：如果内存是连续的并且有一个共同的父对象，则返回 %TRUE。
 */
gboolean
gst_memory_is_span (GstMemory * mem1, GstMemory * mem2, gsize * offset)
{
  g_return_val_if_fail (mem1 != NULL, FALSE);
  g_return_val_if_fail (mem2 != NULL, FALSE);

  /* 必须是相同的内存分配器 */
  if (mem1->allocator != mem2->allocator)
    return FALSE;

  /* need to have the same parent */
  if (mem1->parent == NULL || mem1->parent != mem2->parent)
    return FALSE;

  /* and memory is contiguous */
  if (!mem1->allocator->mem_is_span (mem1, mem2, offset))
    return FALSE;

  return TRUE;
}

void
_priv_gst_memory_initialize (void)
{
  _gst_memory_type = gst_memory_get_type ();
}


GstMemory *
gst_memory_ref (GstMemory * memory)
{
  return (GstMemory *) gst_mini_object_ref (GST_MINI_OBJECT_CAST (memory));
}


void
gst_memory_unref (GstMemory * memory)
{
  gst_mini_object_unref (GST_MINI_OBJECT_CAST (memory));
}
