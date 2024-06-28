#include "gst_private.h"

#include "gstpad.h"
#include "gstpadtemplate.h"
#include "gstenumtypes.h"
#include "gstutils.h"
#include "gstinfo.h"
#include "gsterror.h"
#include "gstvalue.h"

#define GST_CAT_DEFAULT GST_CAT_PADS

enum
{
  PROP_NAME_TEMPLATE = 1,
  PROP_DIRECTION,
  PROP_PRESENCE,
  PROP_CAPS,
  PROP_GTYPE,
};

enum
{
  TEMPL_PAD_CREATED,
  /* FILL ME */
  LAST_SIGNAL
};

static guint gst_pad_template_signals[LAST_SIGNAL] = { 0 };

static void gst_pad_template_dispose (GObject * object);
static void gst_pad_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_pad_template_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

#define gst_pad_template_parent_class parent_class
G_DEFINE_TYPE (GstPadTemplate, gst_pad_template, GST_TYPE_OBJECT);

static void
gst_pad_template_class_init (GstPadTemplateClass * klass)
{
  GObjectClass *gobject_class;
  GstObjectClass *gstobject_class;

  gobject_class = (GObjectClass *) klass;
  gstobject_class = (GstObjectClass *) klass;

  /**
   * GstPadTemplate::pad-created:
   * @pad_template: the object which received the signal.
   * @pad: the pad that was created.
   *
   * 当元素使用该模板创建元素的时候，该信号会被发射
   */
  gst_pad_template_signals[TEMPL_PAD_CREATED] =
      g_signal_new ("pad-created", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstPadTemplateClass, pad_created),
      NULL, NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_PAD);

  gobject_class->dispose = gst_pad_template_dispose;

  gobject_class->get_property = gst_pad_template_get_property;
  gobject_class->set_property = gst_pad_template_set_property;

  /**
   * GstPadTemplate:name-template:
   *
   * The name template of the pad template.
   */
  g_object_class_install_property (gobject_class, PROP_NAME_TEMPLATE,
      g_param_spec_string ("name-template", "Name template",
          "The name template of the pad template", NULL,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * GstPadTemplate:direction:
   *
   * The direction of the pad described by the pad template.
   */
  g_object_class_install_property (gobject_class, PROP_DIRECTION,
      g_param_spec_enum ("direction", "Direction",
          "The direction of the pad described by the pad template",
          GST_TYPE_PAD_DIRECTION, GST_PAD_UNKNOWN,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * GstPadTemplate:presence:
   *
   * When the pad described by the pad template will become available.
   */
  g_object_class_install_property (gobject_class, PROP_PRESENCE,
      g_param_spec_enum ("presence", "Presence",
          "When the pad described by the pad template will become available",
          GST_TYPE_PAD_PRESENCE, GST_PAD_ALWAYS,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * GstPadTemplate:caps:
   *
   * The capabilities of the pad described by the pad template.
   */
  g_object_class_install_property (gobject_class, PROP_CAPS,
      g_param_spec_boxed ("caps", "Caps",
          "The capabilities of the pad described by the pad template",
          GST_TYPE_CAPS,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  /**
   * GstPadTemplate:gtype:
   *
   * The type of the pad described by the pad template.
   *
   * Since: 1.14
   */
  g_object_class_install_property (gobject_class, PROP_GTYPE,
      g_param_spec_gtype ("gtype", "GType",
          "The GType of the pad described by the pad template",
          G_TYPE_NONE,
          G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  gstobject_class->path_string_separator = "*";
}

static void
gst_pad_template_init (GstPadTemplate * templ)
{
  /* GstPadTemplate objects are usually leaked */
  GST_OBJECT_FLAG_SET (templ, GST_OBJECT_FLAG_MAY_BE_LEAKED);
  GST_PAD_TEMPLATE_GTYPE (templ) = G_TYPE_NONE;
}

static void
gst_pad_template_dispose (GObject * object)
{
  GstPadTemplate *templ = GST_PAD_TEMPLATE (object);

  g_free (GST_PAD_TEMPLATE_NAME_TEMPLATE (templ));
  if (GST_PAD_TEMPLATE_CAPS (templ)) {
    gst_caps_unref (GST_PAD_TEMPLATE_CAPS (templ));
  }

  gst_caps_replace (&templ->ABI.abi.documentation_caps, NULL);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

/* ALWAYS padtemplates cannot have conversion specifications (like src_%d),
 * since it doesn't make sense.
 * SOMETIMES padtemplates can do whatever they want, they are provided by the
 * element.
 * REQUEST padtemplates can have multiple specifiers in case of %d and %u, like
 * src_%u_%u, but %s only can be used once in the template.
 */
static gboolean
name_is_valid (const gchar * name, GstPadPresence presence)
{
  const gchar *str, *underscore = NULL;
  gboolean has_s = FALSE;

  if (presence == GST_PAD_ALWAYS) {
    if (strchr (name, '%')) {
      g_warning ("invalid name template %s: conversion specifications are not"
          " allowed for GST_PAD_ALWAYS padtemplates", name);
      return FALSE;
    }
  } else if (presence == GST_PAD_REQUEST) {
    str = strchr (name, '%');

    while (str) {
      if (*(str + 1) != 's' && *(str + 1) != 'd' && *(str + 1) != 'u') {
        g_warning
            ("invalid name template %s: conversion specification must be of"
            " type '%%d', '%%u' or '%%s' for GST_PAD_REQUEST padtemplate",
            name);
        return FALSE;
      }

      if (*(str + 1) == 's' && (*(str + 2) != '\0' || has_s)) {
        g_warning
            ("invalid name template %s: conversion specification of type '%%s'"
            "only can be used once in the GST_PAD_REQUEST padtemplate at the "
            "very end and not allowed any other characters with '%%s'", name);
        return FALSE;
      }

      if (*(str + 1) == 's') {
        has_s = TRUE;
      }

      underscore = strchr (str, '_');
      str = strchr (str + 1, '%');

      if (str && (!underscore || str < underscore)) {
        g_warning
            ("invalid name template %s: each of conversion specifications "
            "must be separated by an underscore", name);
        return FALSE;
      }
    }
  }

  return TRUE;
}

G_DEFINE_POINTER_TYPE (GstStaticPadTemplate, gst_static_pad_template);


/**
 * @name: gst_static_pad_template_get
 * @param pad_template: #GstStaticPadTemplate
 * @brief: 转换 #GstStaticPadTemplate 到 #GstPadTemplate 
 * @fixme: 建议把函数名修改为 gst_pad_template_new_from_static_pad_template()
*/
GstPadTemplate *
gst_static_pad_template_get (GstStaticPadTemplate * pad_template)
{
  GstPadTemplate *new;
  GstCaps *caps;

  if (!name_is_valid (pad_template->name_template, pad_template->presence))
    return NULL;

  caps = gst_static_caps_get (&pad_template->static_caps);

  new = g_object_new (gst_pad_template_get_type (),
      "name", pad_template->name_template,
      "name-template", pad_template->name_template,
      "direction", pad_template->direction,
      "presence", pad_template->presence, "caps", caps, NULL);

  gst_caps_unref (caps);

  return new;
}


/* 使用 @pad_type 转换 #GstStaticPadTemplate 到 #GstPadTemplate  */
GstPadTemplate *
gst_pad_template_new_from_static_pad_template_with_gtype (GstStaticPadTemplate *
    pad_template, GType pad_type)
{
  GstPadTemplate *new;
  GstCaps *caps;

  g_return_val_if_fail (g_type_is_a (pad_type, GST_TYPE_PAD), NULL);

  if (!name_is_valid (pad_template->name_template, pad_template->presence))
    return NULL;

  caps = gst_static_caps_get (&pad_template->static_caps);

  new = g_object_new (gst_pad_template_get_type (),
      "name", pad_template->name_template,
      "name-template", pad_template->name_template,
      "direction", pad_template->direction,
      "presence", pad_template->presence, "caps", caps, "gtype", pad_type,
      NULL);

  gst_caps_unref (caps);

  return new;
}

/**
 * @name gst_pad_template_new
 * @param name_template: the name template.
 * @param direction: the #GstPadDirection of the template.
 * @param presence: the #GstPadPresence of the pad.
 * @caps: (transfer none): a #GstCaps set for the template.
 * @brief: 使用给定的参数创建一个 #GstPadTemplate
 */

GstPadTemplate *
gst_pad_template_new (const gchar * name_template,
    GstPadDirection direction, GstPadPresence presence, GstCaps * caps)
{
  GstPadTemplate *new;

  g_return_val_if_fail (name_template != NULL, NULL);
  g_return_val_if_fail (caps != NULL, NULL);
  g_return_val_if_fail (direction == GST_PAD_SRC
      || direction == GST_PAD_SINK, NULL);
  g_return_val_if_fail (presence == GST_PAD_ALWAYS
      || presence == GST_PAD_SOMETIMES || presence == GST_PAD_REQUEST, NULL);

  if (!name_is_valid (name_template, presence)) {
    return NULL;
  }

  new = g_object_new (gst_pad_template_get_type (),
      "name", name_template, "name-template", name_template,
      "direction", direction, "presence", presence, "caps", caps, NULL);

  return new;
}

/**
 * @name gst_pad_template_new_with_gtype
 * @param name_template: the name template.
 * @param direction: the #GstPadDirection of the template.
 * @param presence: the #GstPadPresence of the pad.
 * @param caps: (transfer none): a #GstCaps set for the template.
 * @param pad_type: The #GType of the pad to create
 * @brief: 使用给定的参数创建一个 #GstPadTemplate
 */
GstPadTemplate *
gst_pad_template_new_with_gtype (const gchar * name_template,
    GstPadDirection direction, GstPadPresence presence, GstCaps * caps,
    GType pad_type)
{
  GstPadTemplate *new;

  g_return_val_if_fail (name_template != NULL, NULL);
  g_return_val_if_fail (caps != NULL, NULL);
  g_return_val_if_fail (direction == GST_PAD_SRC
      || direction == GST_PAD_SINK, NULL);
  g_return_val_if_fail (presence == GST_PAD_ALWAYS
      || presence == GST_PAD_SOMETIMES || presence == GST_PAD_REQUEST, NULL);
  g_return_val_if_fail (g_type_is_a (pad_type, GST_TYPE_PAD), NULL);

  if (!name_is_valid (name_template, presence)) {
    return NULL;
  }

  new = g_object_new (gst_pad_template_get_type (),
      "name", name_template, "name-template", name_template,
      "direction", direction, "presence", presence, "caps", caps,
      "gtype", pad_type, NULL);

  return new;
}

/**
 * gst_static_pad_template_get_caps:
 * @templ: a #GstStaticPadTemplate to get capabilities of.
 *
 * Gets the capabilities of the static pad template.
 *
 * Returns: (transfer full): the #GstCaps of the static pad template.
 * Unref after usage. Since the core holds an additional
 * ref to the returned caps, use gst_caps_make_writable()
 * on the returned caps to modify it.
 */
GstCaps *
gst_static_pad_template_get_caps (GstStaticPadTemplate * templ)
{
  g_return_val_if_fail (templ, NULL);

  return gst_static_caps_get (&templ->static_caps);
}

/**
 * gst_pad_template_get_caps:
 * @templ: a #GstPadTemplate to get capabilities of.
 *
 * Gets the capabilities of the pad template.
 *
 * Returns: (transfer full): the #GstCaps of the pad template.
 * Unref after usage.
 */
GstCaps *
gst_pad_template_get_caps (GstPadTemplate * templ)
{
  GstCaps *caps;
  g_return_val_if_fail (GST_IS_PAD_TEMPLATE (templ), NULL);

  caps = GST_PAD_TEMPLATE_CAPS (templ);

  return (caps ? gst_caps_ref (caps) : NULL);
}

/**
 * gst_pad_template_set_documentation_caps:
 * @templ: the pad template to set documented capabilities on
 * @caps: (transfer none): the documented capabilities
 *
 * Certain elements will dynamically construct the caps of their
 * pad templates. In order not to let environment-specific information
 * into the documentation, element authors should use this method to
 * expose "stable" caps to the reader.
 *
 * Since: 1.18
 */
void
gst_pad_template_set_documentation_caps (GstPadTemplate * templ, GstCaps * caps)
{
  g_return_if_fail (GST_IS_PAD_TEMPLATE (templ));
  g_return_if_fail (GST_IS_CAPS (caps));

  if (caps)
    GST_MINI_OBJECT_FLAG_SET (caps, GST_MINI_OBJECT_FLAG_MAY_BE_LEAKED);
  gst_caps_replace (&(((GstPadTemplate *) (templ))->ABI.abi.documentation_caps),
      caps);
}

/**
 * gst_pad_template_get_documentation_caps:
 * @templ: the pad template to get documented capabilities on
 *
 * See gst_pad_template_set_documentation_caps().
 *
 * Returns: (transfer full): The caps to document. For convenience, this will return
 *   gst_pad_template_get_caps() when no documentation caps were set.
 * Since: 1.18
 */
GstCaps *
gst_pad_template_get_documentation_caps (GstPadTemplate * templ)
{
  g_return_val_if_fail (GST_IS_PAD_TEMPLATE (templ), NULL);

  if (((GstPadTemplate *) (templ))->ABI.abi.documentation_caps)
    return gst_caps_ref (((GstPadTemplate *) (templ))->ABI.abi.
        documentation_caps);
  else
    return gst_pad_template_get_caps (templ);
}

/**
 * gst_pad_template_pad_created:
 * @templ: a #GstPadTemplate that has been created
 * @pad:   the #GstPad that created it
 *
 * Emit the pad-created signal for this template when created by this pad.
 */
void
gst_pad_template_pad_created (GstPadTemplate * templ, GstPad * pad)
{
  g_signal_emit (templ, gst_pad_template_signals[TEMPL_PAD_CREATED], 0, pad);
}

static void
gst_pad_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  /* these properties are all construct-only */
  switch (prop_id) {
    case PROP_NAME_TEMPLATE:
      GST_PAD_TEMPLATE_NAME_TEMPLATE (object) = g_value_dup_string (value);
      break;
    case PROP_DIRECTION:
      GST_PAD_TEMPLATE_DIRECTION (object) =
          (GstPadDirection) g_value_get_enum (value);
      break;
    case PROP_PRESENCE:
      GST_PAD_TEMPLATE_PRESENCE (object) =
          (GstPadPresence) g_value_get_enum (value);
      break;
    case PROP_CAPS:
      GST_PAD_TEMPLATE_CAPS (object) = g_value_dup_boxed (value);
      if (GST_PAD_TEMPLATE_CAPS (object) != NULL) {
        /* GstPadTemplate are usually leaked so are their caps */
        GST_MINI_OBJECT_FLAG_SET (GST_PAD_TEMPLATE_CAPS (object),
            GST_MINI_OBJECT_FLAG_MAY_BE_LEAKED);
      }
      break;
    case PROP_GTYPE:
      GST_PAD_TEMPLATE_GTYPE (object) = g_value_get_gtype (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_pad_template_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  /* these properties are all construct-only */
  switch (prop_id) {
    case PROP_NAME_TEMPLATE:
      g_value_set_string (value, GST_PAD_TEMPLATE_NAME_TEMPLATE (object));
      break;
    case PROP_DIRECTION:
      g_value_set_enum (value, GST_PAD_TEMPLATE_DIRECTION (object));
      break;
    case PROP_PRESENCE:
      g_value_set_enum (value, GST_PAD_TEMPLATE_PRESENCE (object));
      break;
    case PROP_CAPS:
      g_value_set_boxed (value, GST_PAD_TEMPLATE_CAPS (object));
      break;
    case PROP_GTYPE:
      g_value_set_gtype (value, GST_PAD_TEMPLATE_GTYPE (object));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
