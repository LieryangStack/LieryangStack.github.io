/**
 * 部分：gstallocator
 * 标题：GstAllocator
 * 简短描述：分配内存块
 * 参见：#GstMemory
 * 自动排序：否
 * 符号：
 * - GstAllocator
 *
 * 内存通常由分配器通过 gst_allocator_alloc() 方法调用创建。当使用 %NULL 作为分配器时，
 * 将使用默认分配器。
 *
 * 可以通过 gst_allocator_register() 注册新的分配器。分配器通过名称识别，并且可以通过
 * gst_allocator_find() 检索。gst_allocator_set_default() 可用于更改默认分配器。
 *
 * 可以使用 gst_memory_new_wrapped() 创建新的内存，该方法包装在其他地方分配的内存。
 * 
 * @note：这个源文件有两个对象的实现
 *        1. 第一个是抽象对象GstAllocator
 *        2. 另一个是基于GstAllocator的GstAllocatorSysmem
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gst_private.h"
#include "gstmemory.h"

/* lieryang add */
#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC (gst_allocator_debug);
#define GST_CAT_DEFAULT gst_allocator_debug

/********************************************抽象类GstAllocator实现*******************************************/

struct _GstAllocatorPrivate
{
  gpointer dummy;
};

#if defined(MEMORY_ALIGNMENT_MALLOC)
gsize gst_memory_alignment = 7;
#elif defined(MEMORY_ALIGNMENT_PAGESIZE)
/* we fill this in in the _init method */
gsize gst_memory_alignment = 0;
#elif defined(MEMORY_ALIGNMENT)
gsize gst_memory_alignment = MEMORY_ALIGNMENT - 1;
#else
#error "No memory alignment configured"
gsize gst_memory_alignment = 0;
#endif

/* the default allocator */
static GstAllocator *_default_allocator;

static GstAllocator *_sysmem_allocator;

/* registered allocators */
static GRWLock lock;
static GHashTable *allocators;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstAllocator, gst_allocator,
    GST_TYPE_OBJECT);

static void
gst_allocator_class_init (GstAllocatorClass * klass)
{
  GST_DEBUG_CATEGORY_INIT (gst_allocator_debug, "allocator", 0,
      "allocator debug");
}

static GstMemory *
_fallback_mem_copy (GstMemory * mem, gssize offset, gssize size)
{
  GstMemory *copy;
  GstMapInfo sinfo, dinfo;
  GstAllocationParams params = { 0, mem->align, 0, 0, };
  GstAllocator *allocator;

  if (!gst_memory_map (mem, &sinfo, GST_MAP_READ))
    return NULL;

  if (size == -1)
    size = sinfo.size > offset ? sinfo.size - offset : 0;

  /* use the same allocator as the memory we copy  */
  allocator = mem->allocator;
  if (GST_OBJECT_FLAG_IS_SET (allocator, GST_ALLOCATOR_FLAG_CUSTOM_ALLOC))
    allocator = NULL;
  copy = gst_allocator_alloc (allocator, size, &params);

  if (!gst_memory_map (copy, &dinfo, GST_MAP_WRITE)) {
    GST_CAT_WARNING (GST_CAT_MEMORY, "could not write map memory %p", copy);
    gst_allocator_free (mem->allocator, copy);
    gst_memory_unmap (mem, &sinfo);
    return NULL;
  }

  GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
      "memcpy %" G_GSSIZE_FORMAT " memory %p -> %p", size, mem, copy);
  memcpy (dinfo.data, sinfo.data + offset, size);
  gst_memory_unmap (copy, &dinfo);
  gst_memory_unmap (mem, &sinfo);

  return copy;
}

static gboolean
_fallback_mem_is_span (GstMemory * mem1, GstMemory * mem2, gsize * offset)
{
  return FALSE;
}

static void
gst_allocator_init (GstAllocator * allocator)
{
  allocator->priv = gst_allocator_get_instance_private (allocator);

  allocator->mem_copy = _fallback_mem_copy;
  allocator->mem_is_span = _fallback_mem_is_span;
}

G_DEFINE_BOXED_TYPE (GstAllocationParams, gst_allocation_params,
    (GBoxedCopyFunc) gst_allocation_params_copy,
    (GBoxedFreeFunc) gst_allocation_params_free);

/**
 * gst_allocation_params_new:
 *
 * 在堆上创建一个新的 #GstAllocationParams。此函数适用于在 GStreamer 语言绑定中使用。
 * 在你自己的代码中，你可以直接在栈上或在结构体中声明一个 #GstAllocationParams，
 * 并调用 gst_allocation_params_init() 来初始化它。
 *
 * 你不需要对此函数返回的实例调用 gst_allocation_params_init()。
 *
 * 返回值：(transfer full) (not nullable)：一个新的 #GstAllocationParams
 *
 * 自版本 1.20 起。
 */
GstAllocationParams *
gst_allocation_params_new (void)
{
  /* 调用 new() 然后调用 init()，而不是调用 new0()，以防万一 init() 的实现将来改变成除了 memset() 以外的其他操作。 */
  GstAllocationParams *result = g_slice_new (GstAllocationParams);
  gst_allocation_params_init (result);
  return result;
}

/**
 * gst_allocation_params_init:
 * @params: a #GstAllocationParams
 *
 * Initialize @params to its default values
 */
void
gst_allocation_params_init (GstAllocationParams * params)
{
  g_return_if_fail (params != NULL);

  memset (params, 0, sizeof (GstAllocationParams));
}

/**
 * gst_allocation_params_copy:
 * @params: (transfer none) (nullable): 一个 #GstAllocationParams
 *
 * 创建一个 @params 的副本。
 *
 * 返回值：(transfer full) (nullable)：一个新的 #GstAllocationParams。
 */
GstAllocationParams *
gst_allocation_params_copy (const GstAllocationParams * params)
{
  GstAllocationParams *result = NULL;

  if (params) {
    result =
        (GstAllocationParams *) g_slice_copy (sizeof (GstAllocationParams),
        params);
  }
  return result;
}

/**
 * gst_allocation_params_free:
 * @params: (in) (transfer full): 一个 #GstAllocationParams
 *
 * 释放 @params
 */
void
gst_allocation_params_free (GstAllocationParams * params)
{
  g_slice_free (GstAllocationParams, params);
}

/**
 * gst_allocator_register:
 * @name: 分配器的名称
 * @allocator: (transfer full): #GstAllocator
 *
 * 使用 name 注册内存 @allocator。
 */
void
gst_allocator_register (const gchar * name, GstAllocator * allocator)
{
  g_return_if_fail (name != NULL);
  g_return_if_fail (allocator != NULL);

  GST_CAT_DEBUG (GST_CAT_MEMORY, "registering allocator %p with name \"%s\"",
      allocator, name);

  g_rw_lock_writer_lock (&lock);
  /* The ref will never be released */
  GST_OBJECT_FLAG_SET (allocator, GST_OBJECT_FLAG_MAY_BE_LEAKED);
  g_hash_table_insert (allocators, (gpointer) g_strdup (name),
      (gpointer) allocator);
  g_rw_lock_writer_unlock (&lock);
}

/**
 * gst_allocator_find:
 * @name: (nullable): 分配器的名称
 *
 * 查找之前使用 @name 注册的分配器。当 @name 为 %NULL 时，将返回默认分配器。
 *
 * 返回值：(transfer full) (nullable): 一个 #GstAllocator 或在
 * 名为 @name 的分配器未被注册时返回 %NULL。
 */
GstAllocator *
gst_allocator_find (const gchar * name)
{
  GstAllocator *allocator;

  g_rw_lock_reader_lock (&lock);
  if (name) {
    allocator = g_hash_table_lookup (allocators, (gconstpointer) name);
  } else {
    allocator = _default_allocator;
  }
  if (allocator)
    gst_object_ref (allocator);
  g_rw_lock_reader_unlock (&lock);

  return allocator;
}

/**
 * gst_allocator_set_default:
 * @allocator: (transfer full): 一个 #GstAllocator
 *
 * 设置默认分配器。
 */
void
gst_allocator_set_default (GstAllocator * allocator)
{
  GstAllocator *old;

  g_return_if_fail (GST_IS_ALLOCATOR (allocator));

  g_rw_lock_writer_lock (&lock);
  old = _default_allocator;
  _default_allocator = allocator;
  g_rw_lock_writer_unlock (&lock);

  if (old)
    gst_object_unref (old);
}

/**
 * gst_allocator_alloc:
 * @allocator: (transfer none) (nullable): 要使用的 #GstAllocator
 * @size: 可见内存区域的大小
 * @params: (transfer none) (nullable): 可选参数
 *
 * 使用 @allocator 分配一个新的内存块，该内存块至少有 @size 大小。
 *
 * 可选的 @params 可以指定内存的前缀和填充。如果传递 %NULL，将使用默认对齐方式，并且不使用额外的前缀/填充和标志。
 *
 * 如果标志包含 #GST_MEMORY_FLAG_ZERO_PREFIXED 和 #GST_MEMORY_FLAG_ZERO_PADDED，前缀/填充将被填充为 0。
 *
 * 当 @allocator 为 %NULL 时，将使用默认分配器。
 *
 * 在 @params 中的对齐方式是以位掩码给出的，所以 @align + 1 等于对齐的字节数。例如，要对齐到 8 字节，使用 7 作为对齐值。
 *
 * 返回值：(transfer full) (nullable): 一个新的 #GstMemory。
 */
GstMemory *
gst_allocator_alloc (GstAllocator * allocator, gsize size,
                     GstAllocationParams * params)
{
  GstMemory *mem;
  static GstAllocationParams defparams = { 0, 0, 0, 0, };
  GstAllocatorClass *aclass;

  if (params) {
    /* 对齐数必须是2的幂次减一  */
    g_return_val_if_fail (((params->align + 1) & params->align) == 0, NULL);
  } else {
    params = &defparams;
  }

  if (allocator == NULL)
    allocator = _default_allocator;

  aclass = GST_ALLOCATOR_GET_CLASS (allocator);
  if (aclass->alloc)
    mem = aclass->alloc (allocator, size, params);
  else
    mem = NULL;

  return mem;
}

/**
 * gst_allocator_free:
 * @allocator: (transfer none): 要使用的 #GstAllocator
 * @memory: (transfer full): 要释放的内存
 *
 * 释放之前用 gst_allocator_alloc() 分配的 @memory。
 */
void
gst_allocator_free (GstAllocator * allocator, GstMemory * memory)
{
  GstAllocatorClass *aclass;

  g_return_if_fail (GST_IS_ALLOCATOR (allocator));
  g_return_if_fail (memory != NULL);
  g_return_if_fail (memory->allocator == allocator);

  aclass = GST_ALLOCATOR_GET_CLASS (allocator);
  if (aclass->free)
    aclass->free (allocator, memory);
}

/********************************************默认的内存分配器实现GstAllocatorSysmem*******************************************/

/* default memory implementation */
typedef struct
{
  GstMemory mem;

  gsize slice_size;
  guint8 *data;

  gpointer user_data;
  GDestroyNotify notify;
} GstMemorySystem;

typedef struct
{
  GstAllocator parent;
} GstAllocatorSysmem;

typedef struct
{
  GstAllocatorClass parent_class;
} GstAllocatorSysmemClass;

static GType gst_allocator_sysmem_get_type (void);
G_DEFINE_TYPE (GstAllocatorSysmem, gst_allocator_sysmem, GST_TYPE_ALLOCATOR);

/**
 * @berif: 实际调用的是 gst_memory_init， gst_memory_init调用了gst_mini_object_init(在GstMiniObject初始化函数中，ref赋值为1)
 * @calledby: 被_sysmem_new调用（其实这是一个内联函数，会展开到_sysmem_new中），_sysmem_new被
 * @param slice_size: 实际请求的所有内存（slice_size = align + sizeof (GstMemorySystem) + maxsize）
 * @param maxsize: 对齐size + 前缀size + 用户请求size + 填充size
 * @param size: 用户实际请求的size
*/
static inline void
_sysmem_init (GstMemorySystem * mem, GstMemoryFlags flags,
    GstMemory * parent, gsize slice_size,
    gpointer data, gsize maxsize, gsize align, gsize offset, gsize size,
    gpointer user_data, GDestroyNotify notify)
{
  gst_memory_init (GST_MEMORY_CAST (mem),
      flags, _sysmem_allocator, parent, maxsize, align, offset, size);

  mem->slice_size = slice_size;
  mem->data = data;
  mem->user_data = user_data;
  mem->notify = notify;
}

/**
 * @brief: 给GstMemorySystem结构体分配内存空间，用来管理用户数据内存。
 * @calledby: _sysmem_share
 *            _sysmem_is_span
*/
static inline GstMemorySystem *
_sysmem_new (GstMemoryFlags flags,
    GstMemory * parent, gpointer data, gsize maxsize, gsize align, gsize offset,
    gsize size, gpointer user_data, GDestroyNotify notify)
{
  GstMemorySystem *mem;
  gsize slice_size;

  slice_size = sizeof (GstMemorySystem);

  mem = g_slice_alloc (slice_size);
  _sysmem_init (mem, flags, parent, slice_size,
      data, maxsize, align, offset, size, user_data, notify);

  return mem;
}

/*  */
/**
 * @brief: 在一个块中分配内存和结构体
 * @calledby: _sysmem_copy
 *            default_alloc
 * @param maxsize: 前缀size + 用户请求size + 填充size
 * @param align: 对齐size，GstAllocationParams->align
 * @param offset: 前缀size， GstAllocationParams->prefix
 * @param size: 用户请求的内存空间大小
 * @note: slice_size = align + sizeof (GstMemorySystem) + maxsize
*/
static GstMemorySystem *
_sysmem_new_block (GstMemoryFlags flags,
    gsize maxsize, gsize align, gsize offset, gsize size)
{
  GstMemorySystem *mem;
  gsize aoffset, slice_size, padding;
  guint8 *data;

  /* 确保被配置对齐 */
  align |= gst_memory_alignment;
  /* 分配更多以补偿对齐, 这里的maxsize = align + prefix + size + padding */
  maxsize += align;
  /* slice_size = 管理内存结构体大小 + 用户存储数据data大小 */
  slice_size = sizeof (GstMemorySystem) + maxsize;

  mem = g_slice_alloc (slice_size);
  if (mem == NULL)
    return NULL;

  g_print ("gst_memory_alignment = %d\n", gst_memory_alignment);
  g_print ("align = %d\n", align);
  g_print ("maxsize = %ld\n", maxsize);

  data = (guint8 *) mem + sizeof (GstMemorySystem);

  g_print ("data = %p\n", data);

  /* 做对齐 */
  if ((aoffset = ((guintptr) data & align))) {
    aoffset = (align + 1) - aoffset;
    data += aoffset;
    maxsize -= aoffset;
  }

  if (offset && (flags & GST_MEMORY_FLAG_ZERO_PREFIXED))
    memset (data, 0, offset);

  padding = maxsize - (offset + size);
  if (padding && (flags & GST_MEMORY_FLAG_ZERO_PADDED))
    memset (data + offset + size, 0, padding);

  _sysmem_init (mem, flags, NULL, slice_size, data, maxsize,
      align, offset, size, NULL, NULL);

  return mem;
}

static gpointer
_sysmem_map (GstMemorySystem * mem, gsize maxsize, GstMapFlags flags)
{
  return mem->data;
}

static gboolean
_sysmem_unmap (GstMemorySystem * mem)
{
  return TRUE;
}

static GstMemorySystem *
_sysmem_copy (GstMemorySystem * mem, gssize offset, gsize size)
{
  GstMemorySystem *copy;

  if (size == -1)
    size = mem->mem.size > offset ? mem->mem.size - offset : 0;

  copy = _sysmem_new_block (0, size, mem->mem.align, 0, size);
  GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
      "memcpy %" G_GSIZE_FORMAT " memory %p -> %p", size, mem, copy);
  memcpy (copy->data, mem->data + mem->mem.offset + offset, size);

  return copy;
}

static GstMemorySystem *
_sysmem_share (GstMemorySystem * mem, gssize offset, gsize size)
{
  GstMemorySystem *sub;
  GstMemory *parent;

  /* find the real parent */
  if ((parent = mem->mem.parent) == NULL)
    parent = (GstMemory *) mem;

  if (size == -1)
    size = mem->mem.size - offset;

  /* the shared memory is always readonly */
  sub =
      _sysmem_new (GST_MINI_OBJECT_FLAGS (parent) |
      GST_MINI_OBJECT_FLAG_LOCK_READONLY, parent, mem->data, mem->mem.maxsize,
      mem->mem.align, mem->mem.offset + offset, size, NULL, NULL);

  return sub;
}

static gboolean
_sysmem_is_span (GstMemorySystem * mem1, GstMemorySystem * mem2, gsize * offset)
{

  if (offset) {
    GstMemorySystem *parent;

    parent = (GstMemorySystem *) mem1->mem.parent;

    *offset = mem1->mem.offset - parent->mem.offset;
  }

  g_print ("mem1->data = %p\n", mem1->data);
  g_print ("mem1->size = %p\n", mem1->mem.size);
  g_print ("mem2->data = %p\n", mem2->data);
  g_print ("mem1->data + mem1->mem.offset + mem1->mem.size = %p\n", mem1->data + mem1->mem.offset + mem1->mem.size);
  g_print ("mem2->data + mem2->mem.offset = %p\n", mem2->data + mem2->mem.offset);

  /* and memory is contiguous */
  return mem1->data + mem1->mem.offset + mem1->mem.size ==
      mem2->data + mem2->mem.offset;
}

/**
 * @brief: 一块内存申请，一块内存 = 内存管理结构体 + 存储数据内存
 *         GstAllocatorClass类中的alloc虚函数的实现
*/
static GstMemory *
default_alloc (GstAllocator * allocator, gsize size,
    GstAllocationParams * params)
{
  /* 最大内存是 用户请求的size + 前缀size + 填充size */
  gsize maxsize = size + params->prefix + params->padding;

  return (GstMemory *) _sysmem_new_block (params->flags,
      maxsize, params->align, params->prefix, size);
}

/**
 * @brief: 
*/
static void
default_free (GstAllocator * allocator, GstMemory * mem)
{
  GstMemorySystem *dmem = (GstMemorySystem *) mem;
  gsize slice_size;

  if (dmem->notify)
    dmem->notify (dmem->user_data);

  slice_size = dmem->slice_size;

#ifdef USE_POISONING
  /* just poison the structs, not all the data */
  memset (mem, 0xff, sizeof (GstMemorySystem));
#endif

  g_slice_free1 (slice_size, mem);
}

static void
gst_allocator_sysmem_finalize (GObject * obj)
{
  /* Don't raise warnings if we are shutting down */
  if (_default_allocator)
    g_warning ("The default memory allocator was freed!");

  ((GObjectClass *) gst_allocator_sysmem_parent_class)->finalize (obj);
}

static void
gst_allocator_sysmem_class_init (GstAllocatorSysmemClass * klass)
{
  GObjectClass *gobject_class;
  GstAllocatorClass *allocator_class;

  gobject_class = (GObjectClass *) klass;
  allocator_class = (GstAllocatorClass *) klass;

  gobject_class->finalize = gst_allocator_sysmem_finalize;

  allocator_class->alloc = default_alloc;
  allocator_class->free = default_free;
}

static void
gst_allocator_sysmem_init (GstAllocatorSysmem * allocator)
{
  GstAllocator *alloc = GST_ALLOCATOR_CAST (allocator);

  GST_CAT_DEBUG (GST_CAT_MEMORY, "init allocator %p", allocator);

  alloc->mem_type = GST_ALLOCATOR_SYSMEM;
  alloc->mem_map = (GstMemoryMapFunction) _sysmem_map;
  alloc->mem_unmap = (GstMemoryUnmapFunction) _sysmem_unmap;
  alloc->mem_copy = (GstMemoryCopyFunction) _sysmem_copy;
  alloc->mem_share = (GstMemoryShareFunction) _sysmem_share;
  alloc->mem_is_span = (GstMemoryIsSpanFunction) _sysmem_is_span;
}

void
_priv_gst_allocator_initialize (void)
{
  g_rw_lock_init (&lock);
  allocators = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      gst_object_unref);

#ifdef HAVE_GETPAGESIZE
#ifdef MEMORY_ALIGNMENT_PAGESIZE
  gst_memory_alignment = getpagesize () - 1;
#endif
#endif

  GST_CAT_DEBUG (GST_CAT_MEMORY, "memory alignment: %" G_GSIZE_FORMAT,
      gst_memory_alignment);

  _sysmem_allocator = g_object_new (gst_allocator_sysmem_get_type (), NULL);

  /* Clear floating flag */
  gst_object_ref_sink (_sysmem_allocator);

  gst_allocator_register (GST_ALLOCATOR_SYSMEM,
      gst_object_ref (_sysmem_allocator));

  _default_allocator = gst_object_ref (_sysmem_allocator);
}

void
_priv_gst_allocator_cleanup (void)
{
  gst_object_unref (_sysmem_allocator);
  _sysmem_allocator = NULL;

  gst_object_unref (_default_allocator);
  _default_allocator = NULL;

  g_clear_pointer (&allocators, g_hash_table_unref);
}

/**
 * gst_memory_new_wrapped:
 * @brief: 用户提供data内存，然后创建一个GstMemory进行管理该内存空间
 * @flags: #GstMemoryFlags
 * @data: (array length=size) (element-type guint8) (transfer none): 要包装的数据
 * @maxsize: @data 的分配大小
 * @offset: 在 @data 中的偏移量
 * @size: 有效数据的大小
 * @user_data: (nullable): 用户数据
 * @notify: (nullable) (scope async) (closure user_data): 当内存被释放时，用 @user_data 调用此函数
 *
 * 分配一个新的内存块来包装给定的 @data。
 *
 * 如果 @flags 包含 #GST_MEMORY_FLAG_ZERO_PREFIXED 和 #GST_MEMORY_FLAG_ZERO_PADDED，
 * 则前缀/填充必须分别用 0 填充。
 *
 * 返回值：(transfer full) (nullable): 一个新的 #GstMemory。
 */
GstMemory *
gst_memory_new_wrapped (GstMemoryFlags flags, gpointer data,
    gsize maxsize, gsize offset, gsize size, gpointer user_data,
    GDestroyNotify notify)
{
  GstMemorySystem *mem;

  g_return_val_if_fail (data != NULL, NULL);
  g_return_val_if_fail (offset + size <= maxsize, NULL);

  mem =
      _sysmem_new (flags, NULL, data, maxsize, 0, offset, size, user_data,
      notify);

  return (GstMemory *) mem;
}
