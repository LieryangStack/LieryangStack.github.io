#define GST_DISABLE_MINIOBJECT_INLINE_FUNCTIONS
#include "gst_private.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "gstbuffer.h"
#include "gstbufferpool.h"
#include "gstinfo.h"
#include "gstmeta.h"
#include "gstutils.h"
#include "gstversion.h"

/* For g_memdup2 */
#include "glib-compat-private.h"

/* lieryang add */
#include <gst/gst.h>


GType _gst_buffer_type = 0;

/** 
 * struct _GstMetaItem {
 *   GstMetaItem *next;
 *   guint64 seq_num;
 *   GstMeta meta;
 *};
*/
/* info->size是sizeof(FooMeta)，sizeof(FooMeta的第一个成员变量已经包含了GstMeta) 
   GstMetaItem也有一个GstMeta，所以要减去一个GstMeta
 */
#define ITEM_SIZE(info) ((info)->size + sizeof (GstMetaItem) - sizeof (GstMeta))

#define GST_BUFFER_MEM_MAX         16

#define GST_BUFFER_SLICE_SIZE(b)   (((GstBufferImpl *)(b))->slice_size)
#define GST_BUFFER_MEM_LEN(b)      (((GstBufferImpl *)(b))->len)
#define GST_BUFFER_MEM_ARRAY(b)    (((GstBufferImpl *)(b))->mem)
#define GST_BUFFER_MEM_PTR(b,i)    (((GstBufferImpl *)(b))->mem[i])
#define GST_BUFFER_BUFMEM(b)       (((GstBufferImpl *)(b))->bufmem)
#define GST_BUFFER_META(b)         (((GstBufferImpl *)(b))->item)
#define GST_BUFFER_TAIL_META(b)    (((GstBufferImpl *)(b))->tail_item)

/* GstBuffer对象实际创建的结构体 */
typedef struct
{
  GstBuffer buffer; /* 第一个成员变量是GstBuffer */

  gsize slice_size;

  guint len; /* mem存储的GstMemory数量 */
  GstMemory *mem[GST_BUFFER_MEM_MAX];

  /* memory of the buffer when allocated from 1 chunk */
  GstMemory *bufmem;

  /* FIXME, make metadata allocation more efficient by using part of the
   * GstBufferImpl */
  GstMetaItem *item;
  GstMetaItem *tail_item;
} GstBufferImpl;

static gint64 meta_seq;         /* 0 *//* ATOMIC */

/* TODO: use GLib's once https://gitlab.gnome.org/GNOME/glib/issues/1076 lands */
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
static inline gint64
gst_atomic_int64_inc (gint64 * atomic)
{
  return __sync_fetch_and_add (atomic, 1);
}
#elif defined (G_PLATFORM_WIN32)
#include <windows.h>
static inline gint64
gst_atomic_int64_inc (gint64 * atomic)
{
  return InterlockedExchangeAdd64 (atomic, 1);
}
#else
#define STR_TOKEN(s) #s
#define STR(s) STR_TOKEN(s)
#pragma message "No 64-bit atomic int defined for this " STR(TARGET_CPU) " platform/toolchain!"

#define NO_64BIT_ATOMIC_INT_FOR_PLATFORM
G_LOCK_DEFINE_STATIC (meta_seq);
static inline gint64
gst_atomic_int64_inc (gint64 * atomic)
{
  gint64 ret;

  G_LOCK (meta_seq);
  ret = (*atomic)++;
  G_UNLOCK (meta_seq);

  return ret;
}
#endif

/**
 * @brief: 检查从传入的@mem开始，移除两个检测是否存在有连续的data，直到第len个结束
 *         只要有一个组不连续就返回FALSE
 * @param poffset: 第一组连续GstMemory的偏移值被赋值给@poffset
 * @calledby: _actual_merged_memory
*/
static gboolean
_is_span (GstMemory ** mem, gsize len, gsize * poffset, GstMemory ** parent)
{
  GstMemory *mcur, *mprv;
  gboolean have_offset = FALSE;
  gsize i;

  mcur = mprv = NULL;

  for (i = 0; i < len; i++) {
    if (mcur)
      mprv = mcur;
    mcur = mem[i];

    if (mprv && mcur) {
      gsize poffs;

      /* 检查内存是否连续，就是@mprv和@mcur的GstMemory中的data是否连续的 */
      if (!gst_memory_is_span (mprv, mcur, &poffs))
        return FALSE;

      if (!have_offset) {
        if (poffset)
          *poffset = poffs;
        if (parent)
          *parent = mprv->parent;

        have_offset = TRUE;
      }
    }
  }
  return have_offset;
}

/**
 * @brief: 从@idx开始到@length的所有GstMemory合并到一个新的GstMemory（data也申请了一个新的）
*/
static GstMemory *
_actual_merged_memory (GstBuffer * buffer, guint idx, guint length)
{
  GstMemory **mem, *result = NULL;
  GstMemory *parent = NULL;
  gsize size, poffset = 0;

  mem = GST_BUFFER_MEM_ARRAY (buffer);

  size = gst_buffer_get_sizes_range (buffer, idx, length, NULL, NULL);

  if (G_UNLIKELY (_is_span (mem + idx, length, &poffset, &parent))) {
    if (!GST_MEMORY_IS_NO_SHARE (parent))
      result = gst_memory_share (parent, poffset, size);
    if (!result) {
      GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "copy for merge %p", parent);
      result = gst_memory_copy (parent, poffset, size);
    }
  } else {
    gsize i, tocopy, left;
    GstMapInfo sinfo, dinfo;
    guint8 *ptr;

    result = gst_allocator_alloc (NULL, size, NULL);
    if (result == NULL || !gst_memory_map (result, &dinfo, GST_MAP_WRITE)) {
      GST_CAT_ERROR (GST_CAT_BUFFER, "Failed to map memory writable");
      if (result)
        gst_memory_unref (result);
      return NULL;
    }

    ptr = dinfo.data;
    left = size;

    for (i = idx; i < (idx + length) && left > 0; i++) {
      if (!gst_memory_map (mem[i], &sinfo, GST_MAP_READ)) {
        GST_CAT_ERROR (GST_CAT_BUFFER,
            "buffer %p, idx %u, length %u failed to map readable", buffer,
            idx, length);
        gst_memory_unmap (result, &dinfo);
        gst_memory_unref (result);
        return NULL;
      }
      tocopy = MIN (sinfo.size, left);
      GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
          "memcpy %" G_GSIZE_FORMAT " bytes for merge %p from memory %p",
          tocopy, result, mem[i]);
      memcpy (ptr, (guint8 *) sinfo.data, tocopy);
      left -= tocopy;
      ptr += tocopy;
      gst_memory_unmap (mem[i], &sinfo);
    }
    gst_memory_unmap (result, &dinfo);
  }

  return result;
}

/**
 * @brief: 从@idx开始到@length的所有GstMemory合并到一个新的GstMemory（data也申请了一个新的）
*/
static inline GstMemory *
_get_merged_memory (GstBuffer * buffer, guint idx, guint length)
{
  GST_CAT_LOG (GST_CAT_BUFFER, "buffer %p, idx %u, length %u", buffer, idx,
      length);

  if (G_UNLIKELY (length == 0))
    return NULL;

  if (G_LIKELY (length == 1))
    return gst_memory_ref (GST_BUFFER_MEM_PTR (buffer, idx));

  return _actual_merged_memory (buffer, idx, length);
}

/**
 * @brief: 从@idx开始删除@length个位置的GstMemory，如果@mem不为空，idx位置修改为@mem
 * @param len: 一般都是@buffer->len成员的值，即GstMemory的数量
 * @param idx: 要开始处理(GstBufferImpl*)buffer->mem[idx]的索引
 * @param length: 从@idx开始，要处理多少个GstMemory
*/
static void
_replace_memory (GstBuffer * buffer, guint len, guint idx, guint length,
    GstMemory * mem)
{
  gsize end, i;

  end = idx + length;

  GST_CAT_LOG (GST_CAT_BUFFER,
      "buffer %p replace %u-%" G_GSIZE_FORMAT " with memory %p", buffer, idx,
      end, mem);

  /* 解引用之前存储的GstMemory，如果此时GstMemory引用数是1，就会释放掉GstMemory的所有内存 */
  for (i = idx; i < end; i++) {
    GstMemory *old = GST_BUFFER_MEM_PTR (buffer, i);

    gst_memory_unlock (old, GST_LOCK_FLAG_EXCLUSIVE);
    gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (old),
        GST_MINI_OBJECT_CAST (buffer));
    gst_memory_unref (old);
  }

  /* 如果@mem不为空，就把@idx处的GstMemory替换为@mem */
  if (mem != NULL) {
    /* replace with single memory */
    gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (mem),
        GST_MINI_OBJECT_CAST (buffer));
    gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE);
    GST_BUFFER_MEM_PTR (buffer, idx) = mem;
    idx++;
    length--;
  }

  /* 因为中间删除一部分数据，所以要把end后面的指针数组移到idx后面 */
  if (end < len) {
    memmove (&GST_BUFFER_MEM_PTR (buffer, idx),
        &GST_BUFFER_MEM_PTR (buffer, end), (len - end) * sizeof (gpointer));
  }
  GST_BUFFER_MEM_LEN (buffer) = len - length;
  GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY);
}


/* 获取设置在这个GstBuffer上的 #GstBufferFlags 标志 */
GstBufferFlags
gst_buffer_get_flags (GstBuffer * buffer)
{
  return (GstBufferFlags) GST_BUFFER_FLAGS (buffer);
}


/* 检查 GstBuffer上是否被设定了@flags */
gboolean
gst_buffer_has_flags (GstBuffer * buffer, GstBufferFlags flags)
{
  return GST_BUFFER_FLAG_IS_SET (buffer, flags);
}


/* 设定@flags到GstBuffer上 */
gboolean
gst_buffer_set_flags (GstBuffer * buffer, GstBufferFlags flags)
{
  GST_BUFFER_FLAG_SET (buffer, flags);
  return TRUE;
}


/* 删除GstBuffer上的@flags */
gboolean
gst_buffer_unset_flags (GstBuffer * buffer, GstBufferFlags flags)
{
  GST_BUFFER_FLAG_UNSET (buffer, flags);
  return TRUE;
}


/**
 * @brief: 如果@mem还能上独有锁，就返回@mem引用
 *         如果@mem被上独有锁 + 写锁，返回一个新拷贝的GstMemory
*/
static inline GstMemory *
_memory_get_exclusive_reference (GstMemory * mem)
{
  GstMemory *ret = NULL;

  /* 如果@mem还能上独有锁，就返回@mem引用 */
  if (gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE)) {
    ret = gst_memory_ref (mem);
  } else {

    ret = gst_memory_copy (mem, 0, -1);

    if (ret) {
      if (!gst_memory_lock (ret, GST_LOCK_FLAG_EXCLUSIVE)) {
        gst_memory_unref (ret);
        ret = NULL;
      }
    }
  }

  if (!ret)
    GST_CAT_WARNING (GST_CAT_BUFFER, "Failed to acquire an exclusive lock for "
        "memory %p", mem);

  return ret;
}

/**
 * @brief: 添加GstMemory到buffer->mem[idx]
 * @param idx: -1 表示GstMemory插入到最后
 *             idx <= len 插入到mem[idx]
*/
static inline void
_memory_add (GstBuffer * buffer, gint idx, GstMemory * mem)
{
  guint i, len = GST_BUFFER_MEM_LEN (buffer);

  GST_CAT_LOG (GST_CAT_BUFFER, "buffer %p, idx %d, mem %p", buffer, idx, mem);

  if (G_UNLIKELY (len >= GST_BUFFER_MEM_MAX)) {
    /* too many buffer, span them. */
    /* FIXME, there is room for improvement here: We could only try to merge
     * 2 buffers to make some room. If we can't efficiently merge 2 buffers we
     * could try to only merge the two smallest buffers to avoid memcpy, etc. */
    GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "memory array overflow in buffer %p",
        buffer);
    _replace_memory (buffer, len, 0, len, _get_merged_memory (buffer, 0, len));
    /* we now have 1 single spanned buffer */
    len = 1;
  }

  if (idx == -1)
    idx = len;

  /* 检测是否是在len中间插入idx */
  for (i = len; i > idx; i--) {
    /* move buffers to insert, FIXME, we need to insert first and then merge */
    GST_BUFFER_MEM_PTR (buffer, i) = GST_BUFFER_MEM_PTR (buffer, i - 1);
  }
  /* 把GstMemory添加到 buffer->mem[idx] */
  GST_BUFFER_MEM_PTR (buffer, idx) = mem;
  /* buffer->len加一 */
  GST_BUFFER_MEM_LEN (buffer) = len + 1;
  gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (mem),
      GST_MINI_OBJECT_CAST (buffer));

  /* GstBuffer的flag设定GST_BUFFER_FLAG_TAG_MEMORY（表示内存被添加） */
  GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY);
}

GST_DEFINE_MINI_OBJECT_TYPE (GstBuffer, gst_buffer);

/**
 * @calledby: gst_init调用
*/
void
_priv_gst_buffer_initialize (void)
{
  _gst_buffer_type = gst_buffer_get_type ();

#ifdef NO_64BIT_ATOMIC_INT_FOR_PLATFORM
  GST_CAT_WARNING (GST_CAT_PERFORMANCE,
      "No 64-bit atomic int defined for this platform/toolchain!");
#endif
}


/* 获取GstBuffer上最大可以容纳GstMemory的数量 */
guint
gst_buffer_get_max_memory (void)
{
  return GST_BUFFER_MEM_MAX;
}

/**
 * @name gst_buffer_copy_into:
 * @param dest: 目标 #GstBuffer
 * @param src: 源 #GstBuffer
 * @param flags: 指示应该复制哪些元数据字段的标志。
 * @param offset: 从哪里开始复制
 * @param size: 要复制的总大小。如果是 -1，则复制所有数据。
 *
 * @brief: 将 @src 的信息复制到 @dest。
 *
 * 如果 @dest 已经包含内存并且 @flags 包含 GST_BUFFER_COPY_MEMORY，那么 @src 的内存将被附加到 @dest。
 *
 * @flags 指示将复制哪些字段。
 *
 * 返回值：如果复制成功，则返回 %TRUE；否则返回 %FALSE。
 */
gboolean
gst_buffer_copy_into (GstBuffer * dest, GstBuffer * src,
    GstBufferCopyFlags flags, gsize offset, gsize size)
{
  GstMetaItem *walk;
  gsize bufsize;
  gboolean region = FALSE;

  g_return_val_if_fail (dest != NULL, FALSE);
  g_return_val_if_fail (src != NULL, FALSE);

  /* 如果 dest 等于 src，就没有什么要做的 */
  if (G_UNLIKELY (dest == src))
    return TRUE;

  g_return_val_if_fail (gst_buffer_is_writable (dest), FALSE);

  /* 所有GstMemory中实际存储data的大小之和 */
  bufsize = gst_buffer_get_size (src);
  g_return_val_if_fail (bufsize >= offset, FALSE);
  if (offset > 0)
    region = TRUE;
  if (size == -1)
    size = bufsize - offset;
  if (size < bufsize)
    region = TRUE;
  g_return_val_if_fail (bufsize >= offset + size, FALSE);

  GST_CAT_LOG (GST_CAT_BUFFER, "copy %p to %p, offset %" G_GSIZE_FORMAT
      "-%" G_GSIZE_FORMAT "/%" G_GSIZE_FORMAT, src, dest, offset, size,
      bufsize);

  /* 判断是否需要拷贝 flags */
  if (flags & GST_BUFFER_COPY_FLAGS) {  
   
    guint flags_mask = ~GST_BUFFER_FLAG_TAG_MEMORY;
    
    /* 把src除GST_BUFFER_FLAG_TAG_MEMORY以外的所有flags都赋值给dest（如果dest有GST_BUFFER_FLAG_TAG_MEMORY，该flag会保留） */
    GST_MINI_OBJECT_FLAGS (dest) =
        (GST_MINI_OBJECT_FLAGS (src) & flags_mask) |  /* 获得除GST_BUFFER_FLAG_TAG_MEMORY以外的所有flags */
        (GST_MINI_OBJECT_FLAGS (dest) & ~flags_mask); /* 查看dest是否有GST_BUFFER_FLAG_TAG_MEMORY */
  }

  /* 拷贝src中的时间戳变量 */
  if (flags & GST_BUFFER_COPY_TIMESTAMPS) {
    if (offset == 0) {
      GST_BUFFER_PTS (dest) = GST_BUFFER_PTS (src);
      GST_BUFFER_DTS (dest) = GST_BUFFER_DTS (src);
      GST_BUFFER_OFFSET (dest) = GST_BUFFER_OFFSET (src);
      if (size == bufsize) { /* 如果传入@size=-1 或者 传入的@size等于bufsize */
        GST_BUFFER_DURATION (dest) = GST_BUFFER_DURATION (src);
        GST_BUFFER_OFFSET_END (dest) = GST_BUFFER_OFFSET_END (src);
      }
    } else {
      GST_BUFFER_PTS (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_DTS (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_DURATION (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_OFFSET (dest) = GST_BUFFER_OFFSET_NONE;
      GST_BUFFER_OFFSET_END (dest) = GST_BUFFER_OFFSET_NONE;
    }
  }

  /* 拷贝GstMemory */
  if (flags & GST_BUFFER_COPY_MEMORY) {
    gsize skip, left, len, dest_len, i, bsize;
    gboolean deep;

    deep = flags & GST_BUFFER_COPY_DEEP;

    /* 得到有多少个GstMemory */
    len = GST_BUFFER_MEM_LEN (src);
    dest_len = GST_BUFFER_MEM_LEN (dest);
    left = size;
    skip = offset;

    /* copy and make regions of the memory */
    /* 遍历获取src中的每个GstMemory */
    for (i = 0; i < len && left > 0; i++) {

      GstMemory *mem = GST_BUFFER_MEM_PTR (src, i);

      bsize = mem->size;

      if (bsize <= skip) { /* 如果@offset >= 用户实际拥有data的大小 */
        /* 不能拷贝内存 */
        skip -= bsize;
      } else {
        GstMemory *newmem = NULL;
        gsize tocopy;

        /* 一般tocopy都是bsize（当前这个GstMemory存储data的大小） */
        tocopy = MIN (bsize - skip, left); /* 此时left等于src所有data的大小之和 */

        /* 如果要拷贝的size < GstMemory的数据size && 不需要深拷贝 && 该GstMemory能共享 */
        if (tocopy < bsize && !deep && !GST_MEMORY_IS_NO_SHARE (mem)) {

          /* 共享原始data的地址到新的GstMemory，也是newmem */
          newmem = gst_memory_share (mem, skip, tocopy);
          if (newmem) {
            gst_memory_lock (newmem, GST_LOCK_FLAG_EXCLUSIVE);
            skip = 0;
          }
        }

        /* 如果要进行深拷贝 || GstMemory不能共享 || （新的GstMemory为空 && @offset > 0） */
        if (deep || GST_MEMORY_IS_NO_SHARE (mem) || (!newmem && tocopy < bsize)) {
          /* 深拷贝，创建新的data内存空间 */
          newmem = gst_memory_copy (mem, skip, tocopy);
          if (newmem) {
            gst_memory_lock (newmem, GST_LOCK_FLAG_EXCLUSIVE);
            skip = 0;
          }
        } else if (!newmem) {
          /* 如果此时GstMemory处于写锁 + 独有锁状态，newmem = gst_memory_copy (mem, 0, -1);
           * 否则就返回 ref(mem)  
           */
          newmem = _memory_get_exclusive_reference (mem);
        }
        
        /* 如果没有共享或者拷贝成功GstMemory */
        if (!newmem) {
          /* 传入的@idx = GST_BUFFER_MEM_LEN (dest)，该函数不是从@idx处开始移除内存吗？？，现在idx就是len啊！！！ */
          gst_buffer_remove_memory_range (dest, dest_len, -1);
          return FALSE;
        }

        /* 新的GstMemory添加到目标GstBuffer */
        _memory_add (dest, -1, newmem);
        left -= tocopy;
      }
    }

    /* 如果是拷贝合并 GST_BUFFER_COPY_MERGE */
    if (flags & GST_BUFFER_COPY_MERGE) {
      GstMemory *mem;

      len = GST_BUFFER_MEM_LEN (dest);
      mem = _get_merged_memory (dest, 0, len);
      if (!mem) {
        gst_buffer_remove_memory_range (dest, dest_len, -1);
        return FALSE;
      }
      _replace_memory (dest, len, 0, len, mem);
    }
  }

  /* 拷贝元数据metadata */
  if (flags & GST_BUFFER_COPY_META) {
    gboolean deep;

    /* 是否被标记深拷贝 */
    deep = (flags & GST_BUFFER_COPY_DEEP) != 0;

    /* 注意：GstGLSyncMeta 的复制依赖于元数据
     *      现在被复制，也就是在缓冲区数据之后，
     *      所以这必须最后发生 */
    for (walk = GST_BUFFER_META (src); walk; walk = walk->next) {
      GstMeta *meta = &walk->meta;
      const GstMetaInfo *info = meta->info;

      /* Don't copy memory metas if we only copied part of the buffer, didn't
       * copy memories or merged memories. In all these cases the memory
       * structure has changed and the memory meta becomes meaningless.
       */
      if ((region || !(flags & GST_BUFFER_COPY_MEMORY)
              || (flags & GST_BUFFER_COPY_MERGE))
          && gst_meta_api_type_has_tag (info->api, _gst_meta_tag_memory)) {
        GST_CAT_DEBUG (GST_CAT_BUFFER,
            "don't copy memory meta %p of API type %s", meta,
            g_type_name (info->api));
      } else if (deep && gst_meta_api_type_has_tag (info->api,
              _gst_meta_tag_memory_reference)) {
        GST_CAT_DEBUG (GST_CAT_BUFFER,
            "don't copy memory reference meta %p of API type %s", meta,
            g_type_name (info->api));
      } else if (info->transform_func) {
        GstMetaTransformCopy copy_data;

        copy_data.region = region;
        copy_data.offset = offset;
        copy_data.size = size;

        if (!info->transform_func (dest, meta, src,
                _gst_meta_transform_copy, &copy_data)) {
          GST_CAT_ERROR (GST_CAT_BUFFER,
              "failed to copy meta %p of API type %s", meta,
              g_type_name (info->api));
        }
      }
    }
  }

  return TRUE;
}

static GstBuffer *
gst_buffer_copy_with_flags (const GstBuffer * buffer, GstBufferCopyFlags flags)
{
  GstBuffer *copy;

  g_return_val_if_fail (buffer != NULL, NULL);

  /* create a fresh new buffer */
  /* 创建一个新的buffer */
  copy = gst_buffer_new ();

  if (!gst_buffer_copy_into (copy, (GstBuffer *) buffer, flags, 0, -1))
    gst_buffer_replace (&copy, NULL); /* 如果没有拷贝成功，就清空copy中的GstMemory */

  if (copy)
    /* 为什么要移除此flag？？？？ */
    GST_BUFFER_FLAG_UNSET (copy, GST_BUFFER_FLAG_TAG_MEMORY);

  return copy;
}

/**
 * @brief: GstMiniObject->copy虚函数实现
*/
static GstBuffer *
_gst_buffer_copy (const GstBuffer * buffer)
{
  /* GST_BUFFER_COPY_ALL也就是GST_BUFFER_COPY_METADATA | GST_BUFFER_COPY_MEMORY */
  return gst_buffer_copy_with_flags (buffer, GST_BUFFER_COPY_ALL);
}

/* 对于buffer中的数据进行深拷贝（不是ref） */
GstBuffer *
gst_buffer_copy_deep (const GstBuffer * buffer)
{
  return gst_buffer_copy_with_flags (buffer,
      GST_BUFFER_COPY_ALL | GST_BUFFER_COPY_DEEP);
}

/**
 * @brief: GstMiniObject->dispose虚函数实现
 * @note: 如果返回TRUE，GstBuffer就会被调用 _gst_buffer_free 函数
*/
static gboolean
_gst_buffer_dispose (GstBuffer * buffer)
{
  GstBufferPool *pool;

  /* 如果不是GstBufferPool中的GstBuffer，就返回TRUE，就会调用free函数 */
  if ((pool = buffer->pool) == NULL)
    return TRUE;

  /* 不释放GstBuffer，保存buffer存活 */
  gst_buffer_ref (buffer);
  /* 把GstBuffer归还到GstBufferPool */
  GST_CAT_LOG (GST_CAT_BUFFER, "release %p to pool %p", buffer, pool);
  gst_buffer_pool_release_buffer (pool, buffer);

  return FALSE;
}

/**
 * @brief: GstMiniObject->free虚函数实现
*/
static void
_gst_buffer_free (GstBuffer * buffer)
{
  GstMetaItem *walk, *next;
  guint i, len;
  gsize msize;

  g_return_if_fail (buffer != NULL);

  GST_CAT_LOG (GST_CAT_BUFFER, "finalize %p", buffer);

  /* 释放元数据metadata */
  for (walk = GST_BUFFER_META (buffer); walk; walk = next) {
    GstMeta *meta = &walk->meta;
    const GstMetaInfo *info = meta->info;

    /* 调用元数据自己定义的free函数 */
    if (info->free_func)
      info->free_func (meta, buffer);

    next = walk->next;
    /* 释放GstMetaInfo内存  */
    g_slice_free1 (ITEM_SIZE (info), walk);
  }

  /* 获取GstBufferImpl结构体的size */
  msize = GST_BUFFER_SLICE_SIZE (buffer);

  /* 释放GstMemory */
  len = GST_BUFFER_MEM_LEN (buffer);
  for (i = 0; i < len; i++) {
    gst_memory_unlock (GST_BUFFER_MEM_PTR (buffer, i), GST_LOCK_FLAG_EXCLUSIVE);
    gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (GST_BUFFER_MEM_PTR
            (buffer, i)), GST_MINI_OBJECT_CAST (buffer));
    gst_memory_unref (GST_BUFFER_MEM_PTR (buffer, i));
  }

  /* 如果buffer是GstMemory的一部分，我们的msize应该被设定到0 */
  if (msize) {
#ifdef USE_POISONING
    memset (buffer, 0xff, msize);
#endif
    g_slice_free1 (msize, buffer);
  } else {
    /* buffer->bufmem */
    gst_memory_unref (GST_BUFFER_BUFMEM (buffer));
  }
}

/**
 * @brief: 1. 初始化GstBufferImpl结构体成员变量
 *         2. 调用 gst_mini_object_init 函数
*/
static void
gst_buffer_init (GstBufferImpl * buffer, gsize size)
{
  gst_mini_object_init (GST_MINI_OBJECT_CAST (buffer), 0, _gst_buffer_type,
      (GstMiniObjectCopyFunction) _gst_buffer_copy,
      (GstMiniObjectDisposeFunction) _gst_buffer_dispose,
      (GstMiniObjectFreeFunction) _gst_buffer_free);

  GST_BUFFER_SLICE_SIZE (buffer) = size;

  GST_BUFFER (buffer)->pool = NULL;
  GST_BUFFER_PTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DURATION (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_OFFSET (buffer) = GST_BUFFER_OFFSET_NONE;
  GST_BUFFER_OFFSET_END (buffer) = GST_BUFFER_OFFSET_NONE;

  GST_BUFFER_MEM_LEN (buffer) = 0;
  GST_BUFFER_META (buffer) = NULL;
}

/**
 * @name: gst_buffer_new
 * @brief: 创建一个GstBufferImpl，并且初始化
 * @note: 此时的GstBuffer是空的，没有data（GstMemory）
 * @return: 新创建的GstBuffer
 */
GstBuffer *
gst_buffer_new (void)
{
  GstBufferImpl *newbuf;

  newbuf = g_slice_new (GstBufferImpl);
  GST_CAT_LOG (GST_CAT_BUFFER, "new %p", newbuf);

  gst_buffer_init (newbuf, sizeof (GstBufferImpl));

  return GST_BUFFER_CAST (newbuf);
}

/**
 * @name: gst_buffer_new_allocate
 * @param allocator: 分配内存所使用的分配器，NULL使用默认的分配器
 * @param size: 新创建的buffer占用多少字节
 * @param params: 分配器参数选项结构体指针
 */
GstBuffer *
gst_buffer_new_allocate (GstAllocator * allocator, gsize size,
    GstAllocationParams * params)
{
  GstBuffer *newbuf;
  GstMemory *mem;
#if 0
  guint8 *data;
  gsize asize;
#endif

#if 1
  if (size > 0) {
    mem = gst_allocator_alloc (allocator, size, params);
    if (G_UNLIKELY (mem == NULL))
      goto no_memory;
  } else {
    mem = NULL;
  }

  newbuf = gst_buffer_new ();

  if (mem != NULL) {
    /* 给GstMemory上专有锁 */
    gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE);
    _memory_add (newbuf, -1, mem);
  }

  GST_CAT_LOG (GST_CAT_BUFFER,
      "new buffer %p of size %" G_GSIZE_FORMAT " from allocator %p", newbuf,
      size, allocator);
#endif

#if 0
  asize = sizeof (GstBufferImpl) + size;
  data = g_slice_alloc (asize);
  if (G_UNLIKELY (data == NULL))
    goto no_memory;

  newbuf = GST_BUFFER_CAST (data);

  gst_buffer_init ((GstBufferImpl *) data, asize);
  if (size > 0) {
    mem = gst_memory_new_wrapped (0, data + sizeof (GstBufferImpl), NULL,
        size, 0, size);
    _memory_add (newbuf, -1, mem, TRUE);
  }
#endif

#if 0
  /* allocate memory and buffer, it might be interesting to do this but there
   * are many complications. We need to keep the memory mapped to access the
   * buffer fields and the memory for the buffer might be just very slow. We
   * also need to do some more magic to get the alignment right. */
  asize = sizeof (GstBufferImpl) + size;
  mem = gst_allocator_alloc (allocator, asize, align);
  if (G_UNLIKELY (mem == NULL))
    goto no_memory;

  /* map the data part and init the buffer in it, set the buffer size to 0 so
   * that a finalize won't free the buffer */
  data = gst_memory_map (mem, &asize, NULL, GST_MAP_WRITE);
  gst_buffer_init ((GstBufferImpl *) data, 0);
  gst_memory_unmap (mem);

  /* strip off the buffer */
  gst_memory_resize (mem, sizeof (GstBufferImpl), size);

  newbuf = GST_BUFFER_CAST (data);
  GST_BUFFER_BUFMEM (newbuf) = mem;

  if (size > 0)
    _memory_add (newbuf, -1, gst_memory_ref (mem), TRUE);
#endif
  GST_BUFFER_FLAG_UNSET (newbuf, GST_BUFFER_FLAG_TAG_MEMORY);

  return newbuf;

  /* ERRORS */
no_memory:
  {
    GST_CAT_WARNING (GST_CAT_BUFFER,
        "failed to allocate %" G_GSIZE_FORMAT " bytes", size);
    return NULL;
  }
}

/**
 * gst_buffer_new_wrapped_full:
 * @name: gst_buffer_new_wrapped_full
 * @brief: 分配一个新的buffer，创建一个新的GstMemory包装data
 * @flags: #GstMemoryFlags
 * @data: (array length=size) (element-type guint8) (transfer none): data to wrap
 * @maxsize: allocated size of @data
 * @offset: offset in @data
 * @size: size of valid data
 * @user_data: (allow-none): user_data
 * @notify: (allow-none) (scope async) (closure user_data): called with @user_data when the memory is freed
 *
 * Allocates a new buffer that wraps the given memory. @data must point to
 * @maxsize of memory, the wrapped buffer will have the region from @offset and
 * @size visible.
 *
 * When the buffer is destroyed, @notify will be called with @user_data.
 *
 * The prefix/padding must be filled with 0 if @flags contains
 * #GST_MEMORY_FLAG_ZERO_PREFIXED and #GST_MEMORY_FLAG_ZERO_PADDED respectively.
 *
 * Returns: (transfer full): a new #GstBuffer
 */
GstBuffer *
gst_buffer_new_wrapped_full (GstMemoryFlags flags, gpointer data,
    gsize maxsize, gsize offset, gsize size, gpointer user_data,
    GDestroyNotify notify)
{
  GstMemory *mem;
  GstBuffer *newbuf;

  newbuf = gst_buffer_new ();
  mem =
      gst_memory_new_wrapped (flags, data, maxsize, offset, size, user_data,
      notify);
  gst_memory_lock (mem, GST_LOCK_FLAG_EXCLUSIVE);
  _memory_add (newbuf, -1, mem);
  GST_BUFFER_FLAG_UNSET (newbuf, GST_BUFFER_FLAG_TAG_MEMORY);

  return newbuf;
}


/* 创建一个新的buffer包装data，这个data最后会被g_free释放 */
GstBuffer *
gst_buffer_new_wrapped (gpointer data, gsize size)
{
  return gst_buffer_new_wrapped_full (0, data, size, 0, size, data, g_free);
}


GstBuffer *
gst_buffer_new_wrapped_bytes (GBytes * bytes)
{
  guint8 *bytes_data;
  gsize size;

  g_return_val_if_fail (bytes != NULL, NULL);
  bytes_data = (guint8 *) g_bytes_get_data (bytes, &size);
  g_return_val_if_fail (bytes_data != NULL, NULL);

  return gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, bytes_data,
      size, 0, size, g_bytes_ref (bytes), (GDestroyNotify) g_bytes_unref);
}


/* 拷贝一份data，给一个新的GstBuffer */
GstBuffer *
gst_buffer_new_memdup (gconstpointer data, gsize size)
{
  gpointer data2 = g_memdup2 (data, size);

  return gst_buffer_new_wrapped_full (0, data2, size, 0, size, data2, g_free);
}

/* 得到GstMemory内存块的数量 */
guint
gst_buffer_n_memory (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  return GST_BUFFER_MEM_LEN (buffer);
}

/* 最前面追加@mem */
void
gst_buffer_prepend_memory (GstBuffer * buffer, GstMemory * mem)
{
  gst_buffer_insert_memory (buffer, 0, mem);
}

/* 最后面追加@mem */
void
gst_buffer_append_memory (GstBuffer * buffer, GstMemory * mem)
{
  gst_buffer_insert_memory (buffer, -1, mem);
}


/* @idx如果是 -1，表示在最后追加GstMemory */
void
gst_buffer_insert_memory (GstBuffer * buffer, gint idx, GstMemory * mem)
{
  GstMemory *tmp;

  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));
  g_return_if_fail (mem != NULL);
  g_return_if_fail (idx == -1 ||
      (idx >= 0 && idx <= GST_BUFFER_MEM_LEN (buffer)));

  tmp = _memory_get_exclusive_reference (mem);
  g_return_if_fail (tmp != NULL);
  gst_memory_unref (mem);
  _memory_add (buffer, idx, tmp);
}

/* @idx索引位置的GstMemory是否可写，如果可写，返回@idx处的GstMemory
                                 如果不可写做一个可写副本，返回可写副本 */
static GstMemory *
_get_mapped (GstBuffer * buffer, guint idx, GstMapInfo * info,
    GstMapFlags flags)
{
  GstMemory *mem, *mapped;

  mem = gst_memory_ref (GST_BUFFER_MEM_PTR (buffer, idx));

  mapped = gst_memory_make_mapped (mem, info, flags);

  if (mapped != mem) { /* 如果不相等的情况，就是创建了一个新的可写的mapped */
   
    /* 此时要把可修改的GstMemory替换掉不可修改的GstMemory */
    gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (mapped),
        GST_MINI_OBJECT_CAST (buffer));
    gst_memory_lock (mapped, GST_LOCK_FLAG_EXCLUSIVE);
    GST_BUFFER_MEM_PTR (buffer, idx) = mapped;

    /* 解锁旧的的GstMemory */
    gst_memory_unlock (mem, GST_LOCK_FLAG_EXCLUSIVE);
    gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (mem),
        GST_MINI_OBJECT_CAST (buffer));
    GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY);
  }
  gst_memory_unref (mem);

  return mapped;
}

/* 直接获取指定@idx处的GstMemory（不是通过share或者copy） */
GstMemory *
gst_buffer_peek_memory (GstBuffer * buffer, guint idx)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  g_return_val_if_fail (idx < GST_BUFFER_MEM_LEN (buffer), NULL);

  return GST_BUFFER_MEM_PTR (buffer, idx);
}

/* 使用@dix得到指定的GstMemory */
GstMemory *
gst_buffer_get_memory (GstBuffer * buffer, guint idx)
{
  return gst_buffer_get_memory_range (buffer, idx, 1);
}

/* 合并所有的GstMemory */
GstMemory *
gst_buffer_get_all_memory (GstBuffer * buffer)
{
  return gst_buffer_get_memory_range (buffer, 0, -1);
}

/**
 * @brief: 得到一个新的GstMemory，这个新的GstMemory是合并了从@idx开始，@length个GstMemory
 *         如果@length是 -1，则从@idx开始往后的所有GstMemory将会被合并
 * 
*/
GstMemory *
gst_buffer_get_memory_range (GstBuffer * buffer, guint idx, gint length)
{
  guint len;

  GST_CAT_DEBUG (GST_CAT_BUFFER, "idx %u, length %d", idx, length);

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_val_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length > 0 && length + idx <= len), NULL);

  if (length == -1)
    length = len - idx;

  return _get_merged_memory (buffer, idx, length);
}

/* 用@mem替换@buffer中索引@idx处的内存块 */
void
gst_buffer_replace_memory (GstBuffer * buffer, guint idx, GstMemory * mem)
{
  gst_buffer_replace_memory_range (buffer, idx, 1, mem);
}

/* 将@buffer中的所有GstMemory删除，第一个buffer->mem就是@mem */
void
gst_buffer_replace_all_memory (GstBuffer * buffer, GstMemory * mem)
{
  gst_buffer_replace_memory_range (buffer, 0, -1, mem);
}

/**
 * gst_buffer_replace_memory_range:
 * @brief: 如果@mem是NULL，就删除@idx后@length个GstMemory
 *         如果@mem不是NULL，就把@idx后@length个GstMemory替换为一个@mem
 * @buffer: a #GstBuffer.
 * @idx: an index
 * @length: a length, should not be 0
 * @mem: (transfer full): a #GstMemory
 * 
 * @note： 这里的替换并不是所有的都替换成@mem，这样也没有意义，就是idx这个替换成@mem，其余的GstMemory都删除了
 *
 * @buffer should be writable.
 */
void
gst_buffer_replace_memory_range (GstBuffer * buffer, guint idx, gint length,
    GstMemory * mem)
{
  guint len;

  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));

  GST_CAT_DEBUG (GST_CAT_BUFFER, "idx %u, length %d, %p", idx, length, mem);

  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length > 0 && length + idx <= len));

  if (length == -1)
    length = len - idx;

  _replace_memory (buffer, len, idx, length, mem);
}

/* 删除第@idx个GstMemory */
void
gst_buffer_remove_memory (GstBuffer * buffer, guint idx)
{
  gst_buffer_remove_memory_range (buffer, idx, 1);
}

/* 删除所有的GstMemory */
void
gst_buffer_remove_all_memory (GstBuffer * buffer)
{
  if (GST_BUFFER_MEM_LEN (buffer))
    gst_buffer_remove_memory_range (buffer, 0, -1);
}

/**
 * @brief: @idx开始移除@buffer中的@length个内存块。
 *         @length可以是-1，在这种情况下，从@idx开始的所有内存都会被移除。
*/
void
gst_buffer_remove_memory_range (GstBuffer * buffer, guint idx, gint length)
{
  guint len;

  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));

  GST_CAT_DEBUG (GST_CAT_BUFFER, "idx %u, length %d", idx, length);

  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || length + idx <= len);

  if (length == -1)
    length = len - idx; 

  _replace_memory (buffer, len, idx, length, NULL);
}


/**
 * @name: gst_buffer_find_memory
 * @param buffer: a #GstBuffer.
 * @param offset: an offset
 * @param size: a size，如果 -1，就会得到从@idx开始的所有内存块
 * @param idx: (out): pointer to index
 * @param length: (out): pointer to length
 * @param skip: (out): pointer to skip
 * 
 * @brief: 找到符合要求的内存块（一个buffer有很多内存块，但是每个内存块可能不一样）
 *         要求是偏移@offset，能够存储数据大小@size。
 *         如果找到符合要求的内存块，返回TRUE，
 *               @idx是符合要求内存块的起始索引
 *               @length是横跨几个连续的内存块组成的。（这个符合@size的内存块可能是由几个内存块才能组成）
 *               @skip是在@idx内存块上的偏移量
 *  
 * 比如现在我有两个内存块，第0个内存块是5Byte，第1个内存块是10Byte。现在查询offset = 6 size = 5的内存块
 * 此时，@skip返回值就是 1 @idx = 1 @length = 1
*/
gboolean
gst_buffer_find_memory (GstBuffer * buffer, gsize offset, gsize size,
    guint * idx, guint * length, gsize * skip)
{
  guint i, len, found;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (idx != NULL, FALSE);
  g_return_val_if_fail (length != NULL, FALSE);
  g_return_val_if_fail (skip != NULL, FALSE);

  len = GST_BUFFER_MEM_LEN (buffer);

  found = 0;

  /* 遍历buffer中的所有mem */
  for (i = 0; i < len; i++) {
    GstMemory *mem;
    gsize s;

    mem = GST_BUFFER_MEM_PTR (buffer, i);
    /* 这个mem存储data的大小 */
    s = mem->size;

    if (s <= offset) {
      offset -= s; /* 内存块大小小于offset或者是空的内存块，我们应该跳过 */
    } else {
      /* block after offset */
      if (found == 0) {
        /* first block, remember index and offset */
        *idx = i;
        *skip = offset;
        if (size == -1) {
          /* 赋值剩余下几个内存块 */
          *length = len - i;
          return TRUE;
        }
        s -= offset;
        offset = 0;
      }
      /* count the amount of found bytes */
      found += s;
      if (found >= size) {
        /* we have enough bytes */
        *length = i - *idx + 1;
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
 * @name: gst_buffer_is_memory_range_writable
 * @brief: 检查从@idx开始，@length这一段GstMemory是否可写，如果有一个不可，返回FALSE
 *         如果 length = -1，检测从@idx开始的所有内存块
 * @note: 这个函数没有检测@buffer是否可写，检测@buffer是否可写使用 gst_buffer_is_writable()
*/
gboolean
gst_buffer_is_memory_range_writable (GstBuffer * buffer, guint idx, gint length)
{
  guint i, len;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);

  GST_CAT_DEBUG (GST_CAT_BUFFER, "idx %u, length %d", idx, length);

  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_val_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length > 0 && length + idx <= len),
      FALSE);

  if (length == -1)
    len -= idx;
  else
    len = length;

  for (i = 0; i < len; i++) {
    if (!gst_memory_is_writable (GST_BUFFER_MEM_PTR (buffer, i + idx)))
      return FALSE;
  }
  return TRUE;
}


/**
 * @name: gst_buffer_is_all_memory_writable
 * @brief: 检查@buffer的所有内存块是否可写
 * @note: 这个函数没有检测@buffer是否可写，检测@buffer是否可写使用 gst_buffer_is_writable()
*/
gboolean
gst_buffer_is_all_memory_writable (GstBuffer * buffer)
{
  return gst_buffer_is_memory_range_writable (buffer, 0, -1);
}


/**
 * @name: gst_buffer_get_sizes
 * @param offset(out): @idx内存块偏移量（这里是第零个内存块）
 * @param maxsize(out): 输出所有内存块最大maxsize之和
 * @brief: 得到@buffer中所有内存块的和
*/
gsize
gst_buffer_get_sizes (GstBuffer * buffer, gsize * offset, gsize * maxsize)
{
  return gst_buffer_get_sizes_range (buffer, 0, -1, offset, maxsize);
}


/* 得到@buffer所有内存块数据size之和 */
gsize
gst_buffer_get_size (GstBuffer * buffer)
{
  guint i;
  gsize size, len;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  /* FAST PATH */
  len = GST_BUFFER_MEM_LEN (buffer);
  for (i = 0, size = 0; i < len; i++)
    size += GST_BUFFER_MEM_PTR (buffer, i)->size;
  return size;
}


/**
 * @name: gst_buffer_get_sizes_range
 * @param buffer: a #GstBuffer.
 * @param idx: an index
 * @param length: a length 如果 @length是-1，就是全部内存块
 * @param offset: (out) (allow-none): @idx内存块的偏移量offset
 * @param maxsize: (out) (allow-none): 内存块GstMemory->maxsize之和
 * @brief: 
 *        得到从@idx开始，横跨@length个内存块的size和
*/
gsize
gst_buffer_get_sizes_range (GstBuffer * buffer, guint idx, gint length,
    gsize * offset, gsize * maxsize)
{
  guint len;
  gsize size; /* 返回值@return size */
  GstMemory *mem;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_val_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length + idx <= len), 0);

  if (length == -1)
    length = len - idx;

  if (G_LIKELY (length == 1)) {
    /* common case */
    mem = GST_BUFFER_MEM_PTR (buffer, idx);
    size = gst_memory_get_sizes (mem, offset, maxsize);
  } else if (offset == NULL && maxsize == NULL) { /* 如果传入参数@offset和@maxsize都是NULL，执行 */
    /* FAST PATH ! */
    guint i, end;

    size = 0;
    end = idx + length;
    for (i = idx; i < end; i++) {
      mem = GST_BUFFER_MEM_PTR (buffer, i);
      size += mem->size;
    }
  } else { /* 如果 length ！= 1 && offset ！= NULL && maxsize ！= NULL 执行 */
    guint i, end;
    gsize extra, offs;

    end = idx + length;
    size = offs = extra = 0;
    for (i = idx; i < end; i++) {
      gsize s, o, ms;

      mem = GST_BUFFER_MEM_PTR (buffer, i);
      s = gst_memory_get_sizes (mem, &o, &ms);

      if (s) {
        if (size == 0)
          /* first size, take accumulated data before as the offset */
          offs = extra + o;
        /* add sizes */
        size += s;
        /* save the amount of data after this block */
        extra = ms - (o + s);
      } else {
        /* empty block, add as extra */
        extra += ms;
      }
    }
    if (offset)
      *offset = offs;
    if (maxsize)
      *maxsize = offs + size + extra;
  }
  return size;
}


/* 根据@offset和@size更改整个Buffer的偏移量和大小 */
void
gst_buffer_resize (GstBuffer * buffer, gssize offset, gssize size)
{
  gst_buffer_resize_range (buffer, 0, -1, offset, size);
}


/* 改变整个Buffer的内存块大小 */
void
gst_buffer_set_size (GstBuffer * buffer, gssize size)
{
  gst_buffer_resize_range (buffer, 0, -1, 0, size);
}


/**
 * @name: gst_buffer_resize_range
 * @brief: 更改从@idx开始横跨@length个内存块的内存大小到@size
*/
gboolean
gst_buffer_resize_range (GstBuffer * buffer, guint idx, gint length,
    gssize offset, gssize size)
{
  guint i, len, end;
  gsize bsize, bufsize, bufoffs, bufmax;

  g_return_val_if_fail (gst_buffer_is_writable (buffer), FALSE);
  g_return_val_if_fail (size >= -1, FALSE);

  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_val_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length + idx <= len), FALSE);

  if (length == -1)
    length = len - idx;

  /* 整个buffer的内存块大小之和 */
  bufsize = gst_buffer_get_sizes_range (buffer, idx, length, &bufoffs, &bufmax);

  GST_CAT_LOG (GST_CAT_BUFFER, "trim %p %" G_GSSIZE_FORMAT "-%" G_GSSIZE_FORMAT
      " size:%" G_GSIZE_FORMAT " offs:%" G_GSIZE_FORMAT " max:%"
      G_GSIZE_FORMAT, buffer, offset, size, bufsize, bufoffs, bufmax);

  /* we can't go back further than the current offset or past the end of the
   * buffer */
  g_return_val_if_fail ((offset < 0 && bufoffs >= -offset) || (offset >= 0
          && bufoffs + offset <= bufmax), FALSE);
  if (size == -1) {
    g_return_val_if_fail (bufsize >= offset, FALSE);
    size = bufsize - offset;
  }
  g_return_val_if_fail (bufmax >= bufoffs + offset + size, FALSE);

  /* 没有变化 */
  if (offset == 0 && size == bufsize)
    return TRUE;

  end = idx + length;
  /* copy and trim */
  for (i = idx; i < end; i++) {
    GstMemory *mem;
    gsize left, noffs;

    mem = GST_BUFFER_MEM_PTR (buffer, i);
    bsize = mem->size;

    noffs = 0;
    /* last buffer always gets resized to the remaining size */
    /*  */
    if (i + 1 == end)
      left = size;
    else if ((gssize) bsize <= offset) { /* 如果第一个内存块不够抵消偏移量 */
      left = 0;
      /* 因为 offset >= offset，所以noffs > 0 还有偏移量没有去掉*/
      noffs = offset - bsize; /* 传入的偏移量@offset减去idx内存块的size */
      offset = 0;
    }
    /* 修剪其他内存块 */
    else
      left = MIN (bsize - offset, size); /*  */

    if (offset != 0 || left != bsize) {
      if (gst_memory_is_writable (mem)) {
        gst_memory_resize (mem, offset, left);
      } else {
        GstMemory *newmem = NULL;

        if (!GST_MEMORY_IS_NO_SHARE (mem))
          newmem = gst_memory_share (mem, offset, left);

        if (!newmem)
          newmem = gst_memory_copy (mem, offset, left);

        if (newmem == NULL)
          return FALSE;

        gst_mini_object_add_parent (GST_MINI_OBJECT_CAST (newmem),
            GST_MINI_OBJECT_CAST (buffer));
        gst_memory_lock (newmem, GST_LOCK_FLAG_EXCLUSIVE);
        GST_BUFFER_MEM_PTR (buffer, i) = newmem;
        gst_memory_unlock (mem, GST_LOCK_FLAG_EXCLUSIVE);
        gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (mem),
            GST_MINI_OBJECT_CAST (buffer));
        gst_memory_unref (mem);

        GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_TAG_MEMORY);
      }
    }

    offset = noffs;
    size -= left;
  }

  return TRUE;
}

/**
 * @name: gst_buffer_map:
 * @param buffer：一个 #GstBuffer。
 * @param info（out）:关于映射的信息
 * @param flags：映射的标志
 *
 * 使用 @buffer 中所有合并内存块的 #GstMapInfo 填充 @info。
 *
 * @flags 描述了对内存的期望访问方式。当 @flags 为 #GST_MAP_WRITE 时，@buffer 应该是可写的（如从 gst_buffer_is_writable() 返回的那样）。
 *
 * 当 @buffer 可写但内存不是时，将自动创建并返回一个可写副本。缓冲区内存的只读副本也将被这个可写副本替换。
 *
 * 使用完毕后，应使用 gst_buffer_unmap() 对 @info 中的内存进行解映射。
 *
 * 返回：%TRUE 表示映射成功且 @info 包含有效数据。
 */
gboolean
gst_buffer_map (GstBuffer * buffer, GstMapInfo * info, GstMapFlags flags)
{
  return gst_buffer_map_range (buffer, 0, -1, info, flags);
}


/**
 * @name: gst_buffer_map_range
 * @brief: 
 * 
 * 先合并从@dix开始@length个内存块，然后对这个内存块 gst_memory_make_mapped，内存信息赋值给 @info结构体
 * 当 @length 为 -1 时，从 @idx 开始的所有内存块被合并并映射。
*/
gboolean
gst_buffer_map_range (GstBuffer * buffer, guint idx, gint length,
    GstMapInfo * info, GstMapFlags flags)
{
  GstMemory *mem, *nmem;
  gboolean write, writable;
  gsize len;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (info != NULL, FALSE);
  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_val_if_fail ((len == 0 && idx == 0 && length == -1) ||
      (length == -1 && idx < len) || (length > 0
          && length + idx <= len), FALSE);

  GST_CAT_LOG (GST_CAT_BUFFER, "buffer %p, idx %u, length %d, flags %04x",
      buffer, idx, length, flags);

  write = (flags & GST_MAP_WRITE) != 0;
  writable = gst_buffer_is_writable (buffer);

  /* check if we can write when asked for write access */
  if (G_UNLIKELY (write && !writable))
    goto not_writable;

  if (length == -1)
    length = len - idx;

  mem = _get_merged_memory (buffer, idx, length);
  if (G_UNLIKELY (mem == NULL))
    goto no_memory;

  /* now try to map */
  nmem = gst_memory_make_mapped (mem, info, flags);
  if (G_UNLIKELY (nmem == NULL))
    goto cannot_map;

  /* if we merged or when the map returned a different memory, we try to replace
   * the memory in the buffer */
  if (G_UNLIKELY (length > 1 || nmem != mem)) {
    /* if the buffer is writable, replace the memory */
    if (writable) {
      _replace_memory (buffer, len, idx, length, gst_memory_ref (nmem));
    } else {
      if (len > 1) {
        GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
            "temporary mapping for memory %p in buffer %p", nmem, buffer);
      }
    }
  }
  return TRUE;

  /* ERROR */
not_writable:
  {
    GST_WARNING ("write map requested on non-writable buffer");
    g_critical ("write map requested on non-writable buffer");
    memset (info, 0, sizeof (GstMapInfo));
    return FALSE;
  }
no_memory:
  {
    /* empty buffer, we need to return NULL */
    GST_DEBUG ("can't get buffer memory");
    memset (info, 0, sizeof (GstMapInfo));
    return TRUE;
  }
cannot_map:
  {
    GST_DEBUG ("cannot map memory");
    memset (info, 0, sizeof (GstMapInfo));
    return FALSE;
  }
}


void
gst_buffer_unmap (GstBuffer * buffer, GstMapInfo * info)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (info != NULL);

  _gst_buffer_map_info_clear ((GstBufferMapInfo *) info);
}

/**
 * gst_buffer_fill:
 * @buffer：一个 #GstBuffer。
 * @offset：偏移量
 * @src：元素类型 guint8）：源地址
 * @size：要复制src的大小
 *
 * 从 @src 复制 @size 字节到 @buffer 的 @offset 位置。
 *
 * 返回：复制的字节数。当 @buffer 中的数据不足时，这个值可能低于 @size。
 */
gsize
gst_buffer_fill (GstBuffer * buffer, gsize offset, gconstpointer src,
    gsize size)
{
  gsize i, len, left;
  const guint8 *ptr = src;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), 0);
  g_return_val_if_fail (src != NULL || size == 0, 0);

  GST_CAT_LOG (GST_CAT_BUFFER,
      "buffer %p, offset %" G_GSIZE_FORMAT ", size %" G_GSIZE_FORMAT, buffer,
      offset, size);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize tocopy;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_WRITE);
    if (info.size > offset) {
      /* we have enough */
      tocopy = MIN (info.size - offset, left);
      memcpy ((guint8 *) info.data + offset, ptr, tocopy);
      left -= tocopy;
      ptr += tocopy;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}


/* 从 @buffer 中的 @offset开始 复制 @size 字节到 @dest 中 */
gsize
gst_buffer_extract (GstBuffer * buffer, gsize offset, gpointer dest, gsize size)
{
  gsize i, len, left;
  guint8 *ptr = dest;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (dest != NULL, 0);

  GST_CAT_LOG (GST_CAT_BUFFER,
      "buffer %p, offset %" G_GSIZE_FORMAT ", size %" G_GSIZE_FORMAT, buffer,
      offset, size);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize tocopy;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_READ);
    if (info.size > offset) {
      /* we have enough */
      tocopy = MIN (info.size - offset, left);
      memcpy (ptr, (guint8 *) info.data + offset, tocopy);
      left -= tocopy;
      ptr += tocopy;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}

/* 比较从 @buffer 中的 @offset 处开始的 @size 字节与 @mem 中的内存。 */
gint
gst_buffer_memcmp (GstBuffer * buffer, gsize offset, gconstpointer mem,
    gsize size)
{
  gsize i, len;
  const guint8 *ptr = mem;
  gint res = 0;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (mem != NULL, 0);

  GST_CAT_LOG (GST_CAT_BUFFER,
      "buffer %p, offset %" G_GSIZE_FORMAT ", size %" G_GSIZE_FORMAT, buffer,
      offset, size);

  if (G_UNLIKELY (gst_buffer_get_size (buffer) < offset + size))
    return -1;

  len = GST_BUFFER_MEM_LEN (buffer);

  for (i = 0; i < len && size > 0 && res == 0; i++) {
    GstMapInfo info;
    gsize tocmp;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_READ);
    if (info.size > offset) {
      /* we have enough */
      tocmp = MIN (info.size - offset, size);
      res = memcmp (ptr, (guint8 *) info.data + offset, tocmp);
      size -= tocmp;
      ptr += tocmp;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return res;
}

/* 从 @offset 处开始，用 @val 填充 @buffer 中的 @size 字节。 */
gsize
gst_buffer_memset (GstBuffer * buffer, gsize offset, guint8 val, gsize size)
{
  gsize i, len, left;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), 0);

  GST_CAT_LOG (GST_CAT_BUFFER,
      "buffer %p, offset %" G_GSIZE_FORMAT ", val %02x, size %" G_GSIZE_FORMAT,
      buffer, offset, val, size);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize toset;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_WRITE);
    if (info.size > offset) {
      /* we have enough */
      toset = MIN (info.size - offset, left);
      memset ((guint8 *) info.data + offset, val, toset);
      left -= toset;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}

/* 从 @parent 中在 @offset 和 @size 处复制创建一个子缓冲区 */
GstBuffer *
gst_buffer_copy_region (GstBuffer * buffer, GstBufferCopyFlags flags,
    gsize offset, gsize size)
{
  GstBuffer *copy;

  g_return_val_if_fail (buffer != NULL, NULL);

  /* create the new buffer */
  copy = gst_buffer_new ();

  GST_CAT_LOG (GST_CAT_BUFFER, "new region copy %p of %p %" G_GSIZE_FORMAT
      "-%" G_GSIZE_FORMAT, copy, buffer, offset, size);

  if (!gst_buffer_copy_into (copy, buffer, flags, offset, size))
    gst_buffer_replace (&copy, NULL);

  return copy;
}


/* 把@buf2的所有内存块添加到@buf1 */
GstBuffer *
gst_buffer_append (GstBuffer * buf1, GstBuffer * buf2)
{
  return gst_buffer_append_region (buf1, buf2, 0, -1);
}


/* 从@buf2偏移@offset开始，把@buf2的内存块添加到@buf1 */
GstBuffer *
gst_buffer_append_region (GstBuffer * buf1, GstBuffer * buf2, gssize offset,
    gssize size)
{
  gsize i, len;

  g_return_val_if_fail (GST_IS_BUFFER (buf1), NULL);
  g_return_val_if_fail (GST_IS_BUFFER (buf2), NULL);

  buf1 = gst_buffer_make_writable (buf1);
  buf2 = gst_buffer_make_writable (buf2);

  gst_buffer_resize (buf2, offset, size);

  len = GST_BUFFER_MEM_LEN (buf2);
  for (i = 0; i < len; i++) {
    GstMemory *mem;

    mem = GST_BUFFER_MEM_PTR (buf2, i);
    gst_mini_object_remove_parent (GST_MINI_OBJECT_CAST (mem),
        GST_MINI_OBJECT_CAST (buf2));
    GST_BUFFER_MEM_PTR (buf2, i) = NULL;
    _memory_add (buf1, -1, mem);
  }

  GST_BUFFER_MEM_LEN (buf2) = 0;
  GST_BUFFER_FLAG_SET (buf2, GST_BUFFER_FLAG_TAG_MEMORY);
  gst_buffer_unref (buf2);

  return buf1;
}

/**
 * @name: gst_buffer_get_meta
 * @brief: 根据@api获取GstMeta，如果没有找到，则返回NULL。
 * @note: 如果有多个GstMeta使用了同一个API，则返回第一个GstMeta
 *        处理多个GstMeta使用了同一个API，使用函数 gst_buffer_iterate_meta() 或者  gst_buffer_foreach_meta()
 *        然后检查 `meta->info.api` member for the API type.
*/
GstMeta *
gst_buffer_get_meta (GstBuffer * buffer, GType api)
{
  GstMetaItem *item;
  GstMeta *result = NULL;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (api != 0, NULL);

  /* find GstMeta of the requested API */
  for (item = GST_BUFFER_META (buffer); item; item = item->next) {
    GstMeta *meta = &item->meta;
    if (meta->info->api == api) {
      result = meta;
      break;
    }
  }
  return result;
}


/**
 * @brief: @api_type的Metadata有多少个
*/
guint
gst_buffer_get_n_meta (GstBuffer * buffer, GType api_type)
{
  gpointer state = NULL;
  GstMeta *meta;
  guint n = 0;

  while ((meta = gst_buffer_iterate_meta_filtered (buffer, &state, api_type)))
    ++n;

  return n;
}


/**
 * @name: gst_buffer_add_meta
 * @brief: 把@info信息添加到GstBuffer->item
*/
GstMeta *
gst_buffer_add_meta (GstBuffer * buffer, const GstMetaInfo * info,
    gpointer params)
{
  GstMetaItem *item;
  GstMeta *result = NULL;
  gsize size;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), NULL);

  /* slice = sizeof(FooMeta) + sizeof(GstMetaItem) - sizeof(GstMeta) */
  size = ITEM_SIZE (info);

  if (!info->init_func)
    item = g_slice_alloc0 (size);
  else
    item = g_slice_alloc (size);
  
  /* 因为GstMetaItem对象不可见 */
  result = &item->meta;

  /* 其实修改的是GstMetaItem中的GstMeta */
  result->info = info;
  result->flags = GST_META_FLAG_NONE;
  GST_CAT_DEBUG (GST_CAT_BUFFER,
      "alloc metadata %p (%s) of size %" G_GSIZE_FORMAT, result,
      g_type_name (info->type), info->size);

  /* 调用用户定义的GstMeta初始化函数 */
  if (info->init_func)
    if (!info->init_func (result, params, buffer))
      goto init_failed;

  item->seq_num = gst_atomic_int64_inc (&meta_seq);
  item->next = NULL;

  if (!GST_BUFFER_META (buffer)) {
    GST_BUFFER_META (buffer) = item;
    GST_BUFFER_TAIL_META (buffer) = item;
  } else {
    GST_BUFFER_TAIL_META (buffer)->next = item;
    GST_BUFFER_TAIL_META (buffer) = item;
  }

  return result;

init_failed:
  {
    g_slice_free1 (size, item);
    return NULL;
  }
}


/**
 * @brief: 删除GstBuffer中的@meta
 *         如果metadata存在，被删除后返回TRUE，如果没有@meta，返回FALSE
*/
gboolean
gst_buffer_remove_meta (GstBuffer * buffer, GstMeta * meta)
{
  GstMetaItem *walk, *prev;

  g_return_val_if_fail (buffer != NULL, FALSE);
  g_return_val_if_fail (meta != NULL, FALSE);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), FALSE);
  g_return_val_if_fail (!GST_META_FLAG_IS_SET (meta, GST_META_FLAG_LOCKED),
      FALSE);

  /* find the metadata and delete */
  prev = GST_BUFFER_META (buffer);
  for (walk = prev; walk; walk = walk->next) {
    GstMeta *m = &walk->meta;
    if (m == meta) {
      const GstMetaInfo *info = meta->info;

      /* remove from list */
      if (GST_BUFFER_TAIL_META (buffer) == walk) {
        if (prev != walk)
          GST_BUFFER_TAIL_META (buffer) = prev;
        else
          GST_BUFFER_TAIL_META (buffer) = NULL;
      }

      if (GST_BUFFER_META (buffer) == walk)
        GST_BUFFER_META (buffer) = walk->next;
      else
        prev->next = walk->next;

      /* call free_func if any */
      if (info->free_func)
        info->free_func (m, buffer);

      /* and free the slice */
      g_slice_free1 (ITEM_SIZE (info), walk);
      break;
    }
    prev = walk;
  }
  return walk != NULL;
}


/**
 * @name: gst_buffer_iterate_meta
 * @brief: 检索当前@state的下一个GstMetaItem，返回下个GstMetaItem->meta
*/
GstMeta *
gst_buffer_iterate_meta (GstBuffer * buffer, gpointer * state)
{
  GstMetaItem **meta;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (state != NULL, NULL);

  meta = (GstMetaItem **) state;
  if (*meta == NULL)
    *meta = GST_BUFFER_META (buffer);
  else
    *meta = (*meta)->next;

  if (*meta)
    return &(*meta)->meta;
  else
    return NULL;
}


/**
 * @name: gst_buffer_iterate_meta_filtered
 * @brief: 根据传入的GstMetaItem也就是（@state），遍历得到符合@meta_api_type的GstMeta
 * @param state(out): 如果找到符合@meta_api_type的GstMeta，state是GstMeta此时对应的GstMetaItem
*/
GstMeta *
gst_buffer_iterate_meta_filtered (GstBuffer * buffer, gpointer * state,
    GType meta_api_type)
{
  GstMetaItem **meta;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (state != NULL, NULL);

  meta = (GstMetaItem **) state;
  
  if (*meta == NULL) /* 如果传入的 state 指向 NULL */
    /* GstBuffer的第一个item赋值给state */
    *meta = GST_BUFFER_META (buffer);
  else /* 如果传入的state不为空，遍历得到next item */
    *meta = (*meta)->next;

  while (*meta != NULL && (*meta)->meta.info->api != meta_api_type)
    *meta = (*meta)->next;

  if (*meta)
    return &(*meta)->meta;
  else
    return NULL;
}

/* 遍历buffer所有的元数据，调用@func函数，如果此函数返回FALSE，就会停止遍历 */
gboolean
gst_buffer_foreach_meta (GstBuffer * buffer, GstBufferForeachMetaFunc func,
    gpointer user_data)
{
  GstMetaItem *walk, *prev, *next;
  gboolean res = TRUE;

  g_return_val_if_fail (buffer != NULL, FALSE);
  g_return_val_if_fail (func != NULL, FALSE);

  /* find the metadata and delete */
  prev = GST_BUFFER_META (buffer);
  for (walk = prev; walk; walk = next) {
    GstMeta *m, *new;

    m = new = &walk->meta;
    next = walk->next;

    res = func (buffer, &new, user_data);

    if (new == NULL) {
      const GstMetaInfo *info = m->info;

      GST_CAT_DEBUG (GST_CAT_BUFFER, "remove metadata %p (%s)", m,
          g_type_name (info->type));

      g_return_val_if_fail (gst_buffer_is_writable (buffer), FALSE);
      g_return_val_if_fail (!GST_META_FLAG_IS_SET (m, GST_META_FLAG_LOCKED),
          FALSE);

      if (GST_BUFFER_TAIL_META (buffer) == walk) {
        if (prev != walk)
          GST_BUFFER_TAIL_META (buffer) = prev;
        else
          GST_BUFFER_TAIL_META (buffer) = NULL;
      }

      /* remove from list */
      if (GST_BUFFER_META (buffer) == walk)
        prev = GST_BUFFER_META (buffer) = next;
      else
        prev->next = next;

      /* call free_func if any */
      if (info->free_func)
        info->free_func (m, buffer);

      /* and free the slice */
      g_slice_free1 (ITEM_SIZE (info), walk);
    } else {
      prev = walk;
    }
    if (!res)
      break;
  }
  return res;
}

/* 从 @offset 处提取最多 @size 字节的数据，将其复制到新分配的内存中。完成后，必须
 * 使用 g_free() 释放 @dest。 
 */
void
gst_buffer_extract_dup (GstBuffer * buffer, gsize offset, gsize size,
    gpointer * dest, gsize * dest_size)
{
  gsize real_size, alloc_size;

  real_size = gst_buffer_get_size (buffer);

  alloc_size = MIN (real_size - offset, size);
  if (alloc_size == 0) {
    *dest = NULL;
    *dest_size = 0;
  } else {
    *dest = g_malloc (alloc_size);
    *dest_size = gst_buffer_extract (buffer, offset, *dest, size);
  }
}

GST_DEBUG_CATEGORY_STATIC (gst_parent_buffer_meta_debug);

/**
 * 向 @buffer 添加一个 #GstParentBufferMeta，该元数据会在缓冲区被释放之前
 * 保持对 @ref 的引用。
 */
GstParentBufferMeta *
gst_buffer_add_parent_buffer_meta (GstBuffer * buffer, GstBuffer * ref)
{
  GstParentBufferMeta *meta;

  g_return_val_if_fail (GST_IS_BUFFER (ref), NULL);

  meta =
      (GstParentBufferMeta *) gst_buffer_add_meta (buffer,
      GST_PARENT_BUFFER_META_INFO, NULL);

  if (!meta)
    return NULL;

  meta->buffer = gst_buffer_ref (ref);

  return meta;
}

/*******************************GstParentBufferMeta元数据定义***********START*********************************/

static gboolean
_gst_parent_buffer_meta_transform (GstBuffer * dest, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GstParentBufferMeta *dmeta, *smeta;

  smeta = (GstParentBufferMeta *) meta;

  if (GST_META_TRANSFORM_IS_COPY (type)) {
    /* copy over the reference to the parent buffer.
     * Usually, this meta means we need to keep the parent buffer
     * alive because one of the child memories is in use, which
     * might not be the case if memory is deep copied or sub-regioned,
     * but we can't tell, so keep the meta */
    dmeta = gst_buffer_add_parent_buffer_meta (dest, smeta->buffer);
    if (!dmeta)
      return FALSE;

    GST_CAT_DEBUG (gst_parent_buffer_meta_debug,
        "copy buffer reference metadata");
  } else {
    /* return FALSE, if transform type is not supported */
    return FALSE;
  }
  return TRUE;
}

static void
_gst_parent_buffer_meta_free (GstParentBufferMeta * parent_meta,
    GstBuffer * buffer)
{
  GST_CAT_DEBUG (gst_parent_buffer_meta_debug,
      "Dropping reference on buffer %p", parent_meta->buffer);
  gst_buffer_unref (parent_meta->buffer);
}

static gboolean
_gst_parent_buffer_meta_init (GstParentBufferMeta * parent_meta,
    gpointer params, GstBuffer * buffer)
{
  static gsize _init;

  if (g_once_init_enter (&_init)) {
    GST_DEBUG_CATEGORY_INIT (gst_parent_buffer_meta_debug, "parentbuffermeta",
        0, "parentbuffermeta");
    g_once_init_leave (&_init, 1);
  }

  parent_meta->buffer = NULL;

  return TRUE;
}

/**
 * gst_parent_buffer_meta_api_get_type: (attributes doc.skip=true)
 */
GType
gst_parent_buffer_meta_api_get_type (void)
{
  static GType type = 0;
  static const gchar *tags[] = { GST_META_TAG_MEMORY_REFERENCE_STR, NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstParentBufferMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }

  return type;
}

/**
 * gst_parent_buffer_meta_get_info:
 *
 * Gets the global #GstMetaInfo describing  the #GstParentBufferMeta meta.
 *
 * Returns: (transfer none): The #GstMetaInfo
 *
 * Since: 1.6
 */
const GstMetaInfo *
gst_parent_buffer_meta_get_info (void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter ((GstMetaInfo **) & meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (gst_parent_buffer_meta_api_get_type (),
        "GstParentBufferMeta",
        sizeof (GstParentBufferMeta),
        (GstMetaInitFunction) _gst_parent_buffer_meta_init,
        (GstMetaFreeFunction) _gst_parent_buffer_meta_free,
        _gst_parent_buffer_meta_transform);
    g_once_init_leave ((GstMetaInfo **) & meta_info, (GstMetaInfo *) meta);
  }

  return meta_info;
}

/*******************************GstParentBufferMeta元数据定义***********END*********************************/

GST_DEBUG_CATEGORY_STATIC (gst_reference_timestamp_meta_debug);

/**
 * gst_buffer_add_reference_timestamp_meta:
 * @buffer: (transfer none): a #GstBuffer
 * @reference: (transfer none): identifier for the timestamp reference.
 * @timestamp: timestamp
 * @duration: duration, or %GST_CLOCK_TIME_NONE
 *
 * Adds a #GstReferenceTimestampMeta to @buffer that holds a @timestamp and
 * optionally @duration based on a specific timestamp @reference. See the
 * documentation of #GstReferenceTimestampMeta for details.
 *
 * Returns: (transfer none) (nullable): The #GstReferenceTimestampMeta that was added to the buffer
 *
 * Since: 1.14
 */
GstReferenceTimestampMeta *
gst_buffer_add_reference_timestamp_meta (GstBuffer * buffer,
    GstCaps * reference, GstClockTime timestamp, GstClockTime duration)
{
  GstReferenceTimestampMeta *meta;

  g_return_val_if_fail (GST_IS_CAPS (reference), NULL);
  g_return_val_if_fail (timestamp != GST_CLOCK_TIME_NONE, NULL);

  meta =
      (GstReferenceTimestampMeta *) gst_buffer_add_meta (buffer,
      GST_REFERENCE_TIMESTAMP_META_INFO, NULL);

  if (!meta)
    return NULL;

  meta->reference = gst_caps_ref (reference);
  meta->timestamp = timestamp;
  meta->duration = duration;

  return meta;
}

/**
 * gst_buffer_get_reference_timestamp_meta:
 * @buffer: a #GstBuffer
 * @reference: (allow-none): a reference #GstCaps
 *
 * Finds the first #GstReferenceTimestampMeta on @buffer that conforms to
 * @reference. Conformance is tested by checking if the meta's reference is a
 * subset of @reference.
 *
 * Buffers can contain multiple #GstReferenceTimestampMeta metadata items.
 *
 * Returns: (transfer none) (nullable): the #GstReferenceTimestampMeta or %NULL when there
 * is no such metadata on @buffer.
 *
 * Since: 1.14
 */
GstReferenceTimestampMeta *
gst_buffer_get_reference_timestamp_meta (GstBuffer * buffer,
    GstCaps * reference)
{
  gpointer state = NULL;
  GstMeta *meta;
  const GstMetaInfo *info = GST_REFERENCE_TIMESTAMP_META_INFO;

  while ((meta = gst_buffer_iterate_meta (buffer, &state))) {
    if (meta->info->api == info->api) {
      GstReferenceTimestampMeta *rmeta = (GstReferenceTimestampMeta *) meta;

      if (!reference)
        return rmeta;
      if (gst_caps_is_subset (rmeta->reference, reference))
        return rmeta;
    }
  }
  return NULL;
}

static gboolean
_gst_reference_timestamp_meta_transform (GstBuffer * dest, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GstReferenceTimestampMeta *dmeta, *smeta;

  /* we copy over the reference timestamp meta, independent of transformation
   * that happens. If it applied to the original buffer, it still applies to
   * the new buffer as it refers to the time when the media was captured */
  smeta = (GstReferenceTimestampMeta *) meta;
  dmeta =
      gst_buffer_add_reference_timestamp_meta (dest, smeta->reference,
      smeta->timestamp, smeta->duration);
  if (!dmeta)
    return FALSE;

  GST_CAT_DEBUG (gst_reference_timestamp_meta_debug,
      "copy reference timestamp metadata from buffer %p to %p", buffer, dest);

  return TRUE;
}

static void
_gst_reference_timestamp_meta_free (GstReferenceTimestampMeta * meta,
    GstBuffer * buffer)
{
  if (meta->reference)
    gst_caps_unref (meta->reference);
}

static gboolean
_gst_reference_timestamp_meta_init (GstReferenceTimestampMeta * meta,
    gpointer params, GstBuffer * buffer)
{
  static gsize _init;

  if (g_once_init_enter (&_init)) {
    GST_DEBUG_CATEGORY_INIT (gst_reference_timestamp_meta_debug,
        "referencetimestampmeta", 0, "referencetimestampmeta");
    g_once_init_leave (&_init, 1);
  }

  meta->reference = NULL;
  meta->timestamp = GST_CLOCK_TIME_NONE;
  meta->duration = GST_CLOCK_TIME_NONE;

  return TRUE;
}

/**
 * gst_reference_timestamp_meta_api_get_type: (attributes doc.skip=true)
 */
GType
gst_reference_timestamp_meta_api_get_type (void)
{
  static GType type = 0;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type =
        gst_meta_api_type_register ("GstReferenceTimestampMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }

  return type;
}

/**
 * gst_reference_timestamp_meta_get_info:
 *
 * Gets the global #GstMetaInfo describing the #GstReferenceTimestampMeta meta.
 *
 * Returns: (transfer none): The #GstMetaInfo
 *
 * Since: 1.14
 */
const GstMetaInfo *
gst_reference_timestamp_meta_get_info (void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter ((GstMetaInfo **) & meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (gst_reference_timestamp_meta_api_get_type (),
        "GstReferenceTimestampMeta",
        sizeof (GstReferenceTimestampMeta),
        (GstMetaInitFunction) _gst_reference_timestamp_meta_init,
        (GstMetaFreeFunction) _gst_reference_timestamp_meta_free,
        _gst_reference_timestamp_meta_transform);
    g_once_init_leave ((GstMetaInfo **) & meta_info, (GstMetaInfo *) meta);
  }

  return meta_info;
}

/**
 * gst_buffer_add_custom_meta:
 * @buffer: (transfer none): a #GstBuffer
 * @name: the registered name of the desired custom meta
 *
 * Creates and adds a #GstCustomMeta for the desired @name. @name must have
 * been successfully registered with gst_meta_register_custom().
 *
 * Returns: (transfer none) (nullable): The #GstCustomMeta that was added to the buffer
 *
 * Since: 1.20
 */
GstCustomMeta *
gst_buffer_add_custom_meta (GstBuffer * buffer, const gchar * name)
{
  GstCustomMeta *meta;
  const GstMetaInfo *info;

  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  info = gst_meta_get_info (name);

  if (info == NULL || !gst_meta_info_is_custom (info))
    return NULL;

  meta = (GstCustomMeta *) gst_buffer_add_meta (buffer, info, NULL);

  return meta;
}

/**
 * gst_buffer_get_custom_meta:
 * @buffer: a #GstBuffer
 * @name: the registered name of the custom meta to retrieve.
 *
 * Finds the first #GstCustomMeta on @buffer for the desired @name.
 *
 * Returns: (transfer none) (nullable): the #GstCustomMeta
 *
 * Since: 1.20
 */
GstCustomMeta *
gst_buffer_get_custom_meta (GstBuffer * buffer, const gchar * name)
{
  const GstMetaInfo *info;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (name != NULL, NULL);

  info = gst_meta_get_info (name);

  if (!info)
    return NULL;

  if (!gst_meta_info_is_custom (info))
    return NULL;

  return (GstCustomMeta *) gst_buffer_get_meta (buffer, info->api);
}

/**
 * gst_buffer_ref: (skip)
 * @buf: a #GstBuffer.
 *
 * Increases the refcount of the given buffer by one.
 *
 * Note that the refcount affects the writability
 * of @buf and its metadata, see gst_buffer_is_writable().
 * It is important to note that keeping additional references to
 * GstBuffer instances can potentially increase the number
 * of `memcpy` operations in a pipeline.
 *
 * Returns: (transfer full): @buf
 */
GstBuffer *
gst_buffer_ref (GstBuffer * buf)
{
  return (GstBuffer *) gst_mini_object_ref (GST_MINI_OBJECT_CAST (buf));
}

/**
 * gst_buffer_unref: (skip)
 * @buf: (transfer full): a #GstBuffer.
 *
 * Decreases the refcount of the buffer. If the refcount reaches 0, the buffer
 * with the associated metadata and memory will be freed.
 */
void
gst_buffer_unref (GstBuffer * buf)
{
  gst_mini_object_unref (GST_MINI_OBJECT_CAST (buf));
}


void
gst_clear_buffer (GstBuffer ** buf_ptr)
{
  gst_clear_mini_object ((GstMiniObject **) buf_ptr);
}

/**
 * gst_buffer_copy: (skip)
 * @buf: a #GstBuffer.
 *
 * Creates a copy of the given buffer. This will only copy the buffer's
 * data to a newly allocated memory if needed (if the type of memory
 * requires it), otherwise the underlying data is just referenced.
 * Check gst_buffer_copy_deep() if you want to force the data
 * to be copied to newly allocated memory.
 *
 * Returns: (transfer full) (nullable): a new copy of @buf if the copy succeeded, %NULL otherwise.
 */
GstBuffer *
gst_buffer_copy (const GstBuffer * buf)
{
  return GST_BUFFER (gst_mini_object_copy (GST_MINI_OBJECT_CONST_CAST (buf)));
}

/**
 * gst_buffer_replace: (skip)
 * @obuf: (inout) (transfer full) (nullable): pointer to a pointer to
 *     a #GstBuffer to be replaced.
 * @nbuf: (transfer none) (allow-none): pointer to a #GstBuffer that will
 *     replace the buffer pointed to by @obuf.
 *
 * Modifies a pointer to a #GstBuffer to point to a different #GstBuffer. The
 * modification is done atomically (so this is useful for ensuring thread safety
 * in some cases), and the reference counts are updated appropriately (the old
 * buffer is unreffed, the new is reffed).
 *
 * Either @nbuf or the #GstBuffer pointed to by @obuf may be %NULL.
 *
 * Returns: %TRUE when @obuf was different from @nbuf.
 */
gboolean
gst_buffer_replace (GstBuffer ** obuf, GstBuffer * nbuf)
{
  return gst_mini_object_replace ((GstMiniObject **) obuf,
      (GstMiniObject *) nbuf);
}
