#define GST_DISABLE_MINIOBJECT_INLINE_FUNCTIONS
#include "gst_private.h"
#include <string.h>
#include "gstcontext.h"
#include "gstquark.h"

struct _GstContext
{
  GstMiniObject mini_object;

  gchar *context_type;
  GstStructure *structure;
  gboolean persistent;
};

#define GST_CONTEXT_STRUCTURE(c)  (((GstContext *)(c))->structure)

GType _gst_context_type = 0;
GST_DEFINE_MINI_OBJECT_TYPE (GstContext, gst_context);

void
_priv_gst_context_initialize (void)
{
  GST_CAT_INFO (GST_CAT_GST_INIT, "init contexts");

  gst_context_get_type ();

  _gst_context_type = gst_context_get_type ();
}

static void
_gst_context_free (GstContext * context)
{
  GstStructure *structure;

  g_return_if_fail (context != NULL);

  GST_CAT_LOG (GST_CAT_CONTEXT, "finalize context %p: %" GST_PTR_FORMAT,
      context, GST_CONTEXT_STRUCTURE (context));

  structure = GST_CONTEXT_STRUCTURE (context);
  if (structure) {
    gst_structure_set_parent_refcount (structure, NULL);
    gst_structure_free (structure);
  }
  g_free (context->context_type);

#ifdef USE_POISONING
  memset (context, 0xff, sizeof (GstContext));
#endif

  g_slice_free1 (sizeof (GstContext), context);
}

static void gst_context_init (GstContext * context);

static GstContext *
_gst_context_copy (GstContext * context)
{
  GstContext *copy;
  GstStructure *structure;

  GST_CAT_LOG (GST_CAT_CONTEXT, "copy context %p: %" GST_PTR_FORMAT, context,
      GST_CONTEXT_STRUCTURE (context));

  copy = g_slice_new0 (GstContext);

  gst_context_init (copy);

  copy->context_type = g_strdup (context->context_type);

  structure = GST_CONTEXT_STRUCTURE (context);
  GST_CONTEXT_STRUCTURE (copy) = gst_structure_copy (structure);
  gst_structure_set_parent_refcount (GST_CONTEXT_STRUCTURE (copy),
      &copy->mini_object.refcount);

  copy->persistent = context->persistent;

  return GST_CONTEXT_CAST (copy);
}

static void
gst_context_init (GstContext * context)
{
  gst_mini_object_init (GST_MINI_OBJECT_CAST (context), 0, _gst_context_type,
      (GstMiniObjectCopyFunction) _gst_context_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_context_free);
}


/**
 * @name: gst_context_new
 * @param context_type: 上下文类型
 * @param persistent: 是否是持久的（持久context在元素达到GST_STATE_NULL会保留）
 * @brief: 创建一个新的上下文
 * @note: 持久的GstContext在元素达到GST_STATE_NULL时会被保留，非持久的上下文将被移除。
 *        此外，非持久的上下文不会覆盖之前设置到元素上的持久上下文
*/
GstContext *
gst_context_new (const gchar * context_type, gboolean persistent)
{
  GstContext *context;
  GstStructure *structure;

  g_return_val_if_fail (context_type != NULL, NULL);

  context = g_slice_new0 (GstContext);

  GST_CAT_LOG (GST_CAT_CONTEXT, "creating new context %p", context);

  structure = gst_structure_new_id_empty (GST_QUARK (CONTEXT));
  gst_structure_set_parent_refcount (structure, &context->mini_object.refcount);
  gst_context_init (context);

  context->context_type = g_strdup (context_type);
  GST_CONTEXT_STRUCTURE (context) = structure;
  context->persistent = persistent;

  return context;
}

/* 得到上下文的类型 */
const gchar *
gst_context_get_context_type (const GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);

  return context->context_type;
}


/* 判断@context是否是@context_type类型 */
gboolean
gst_context_has_context_type (const GstContext * context,
    const gchar * context_type)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), FALSE);
  g_return_val_if_fail (context_type != NULL, FALSE);

  return strcmp (context->context_type, context_type) == 0;
}


/* 获取到上下文的GstStructure，这个返回值你不该应该修改或者释放 */
const GstStructure *
gst_context_get_structure (const GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);

  return GST_CONTEXT_STRUCTURE (context);
}

/**
 * gst_context_writable_structure:
 * @context: 要访问的 #GstContext。
 *
 * 获取上下文结构的可写版本。
 *
 * 返回: (transfer none): 上下文的结构。结构仍然由上下文拥有，这意味着您不应该释放它，并且在释放上下文时指针将变为无效。
 * 此函数检查 @context 是否可写。
 *
 * 自版本: 1.2
 */
GstStructure *
gst_context_writable_structure (GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);
  g_return_val_if_fail (gst_context_is_writable (context), NULL);

  return GST_CONTEXT_STRUCTURE (context);
}

/* 判断是否是持久性上下文 */
gboolean
gst_context_is_persistent (const GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), FALSE);

  return context->persistent;
}

/**
 * gst_context_ref:
 * @context: the context to ref
 *
 * Convenience macro to increase the reference count of the context.
 *
 * Returns: @context (for convenience when doing assignments)
 *
 * Since: 1.2
 */
GstContext *
gst_context_ref (GstContext * context)
{
  return (GstContext *) gst_mini_object_ref (GST_MINI_OBJECT_CAST (context));
}

/**
 * gst_context_unref:
 * @context: the context to unref
 *
 * Convenience macro to decrease the reference count of the context, possibly
 * freeing it.
 *
 * Since: 1.2
 */
void
gst_context_unref (GstContext * context)
{
  gst_mini_object_unref (GST_MINI_OBJECT_CAST (context));
}

/**
 * gst_context_copy:
 * @context: the context to copy
 *
 * Creates a copy of the context. Returns a copy of the context.
 *
 * Returns: (transfer full): a new copy of @context.
 *
 * MT safe
 *
 * Since: 1.2
 */
GstContext *
gst_context_copy (const GstContext * context)
{
  return
      GST_CONTEXT_CAST (gst_mini_object_copy (GST_MINI_OBJECT_CONST_CAST
          (context)));
}

/**
 * gst_context_replace:
 * @old_context: (inout) (transfer full): pointer to a pointer to a #GstContext
 *     to be replaced.
 * @new_context: (allow-none) (transfer none): pointer to a #GstContext that will
 *     replace the context pointed to by @old_context.
 *
 * Modifies a pointer to a #GstContext to point to a different #GstContext. The
 * modification is done atomically (so this is useful for ensuring thread safety
 * in some cases), and the reference counts are updated appropriately (the old
 * context is unreffed, the new one is reffed).
 *
 * Either @new_context or the #GstContext pointed to by @old_context may be %NULL.
 *
 * Returns: %TRUE if @new_context was different from @old_context
 *
 * Since: 1.2
 */
gboolean
gst_context_replace (GstContext ** old_context, GstContext * new_context)
{
  return gst_mini_object_replace ((GstMiniObject **) old_context,
      (GstMiniObject *) new_context);
}
