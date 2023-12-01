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

  /* the GstMiniObject types need to be class_ref'd once before it can be
   * done from multiple threads;
   * see http://bugzilla.gnome.org/show_bug.cgi?id=304551 */
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
 * gst_context_new:
 * @context_type: Context type
 * @persistent: Persistent context
 *
 * Creates a new context.
 *
 * Returns: (transfer full): The new context.
 *
 * Since: 1.2
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

/**
 * gst_context_get_context_type:
 * @context: The #GstContext.
 *
 * Gets the type of @context.
 *
 * Returns: The type of the context.
 *
 * Since: 1.2
 */
const gchar *
gst_context_get_context_type (const GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);

  return context->context_type;
}

/**
 * gst_context_has_context_type:
 * @context: The #GstContext.
 * @context_type: Context type to check.
 *
 * Checks if @context has @context_type.
 *
 * Returns: %TRUE if @context has @context_type.
 *
 * Since: 1.2
 */
gboolean
gst_context_has_context_type (const GstContext * context,
    const gchar * context_type)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), FALSE);
  g_return_val_if_fail (context_type != NULL, FALSE);

  return strcmp (context->context_type, context_type) == 0;
}

/**
 * gst_context_get_structure:
 * @context: The #GstContext.
 *
 * Accesses the structure of the context.
 *
 * Returns: (transfer none): The structure of the context. The structure is
 * still owned by the context, which means that you should not modify it,
 * free it and that the pointer becomes invalid when you free the context.
 *
 * Since: 1.2
 */
const GstStructure *
gst_context_get_structure (const GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);

  return GST_CONTEXT_STRUCTURE (context);
}

/**
 * gst_context_writable_structure:
 * @context: The #GstContext.
 *
 * Gets a writable version of the structure.
 *
 * Returns: (transfer none): The structure of the context. The structure is still
 * owned by the context, which means that you should not free it and
 * that the pointer becomes invalid when you free the context.
 * This function checks if @context is writable.
 *
 * Since: 1.2
 */
GstStructure *
gst_context_writable_structure (GstContext * context)
{
  g_return_val_if_fail (GST_IS_CONTEXT (context), NULL);
  g_return_val_if_fail (gst_context_is_writable (context), NULL);

  return GST_CONTEXT_STRUCTURE (context);
}

/**
 * gst_context_is_persistent:
 * @context: The #GstContext.
 *
 * Checks if @context is persistent.
 *
 * Returns: %TRUE if the context is persistent.
 *
 * Since: 1.2
 */
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
