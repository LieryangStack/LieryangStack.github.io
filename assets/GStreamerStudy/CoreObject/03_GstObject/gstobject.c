/**
 * SECTION: gstobject
 * @title: GstObject
 * @short_description: GStreamer 对象层次结构的基类
 *
 * # GstObject 为 GStreamer 库填充的对象层次结构树提供了根。目前它只是在 #GInitiallyUnowned 之上的薄包装器。这是一个抽象类，单独使用时不太可用。
 *
 * # GstObject 提供了基本的引用计数、父级功能和锁定。大多数函数只是为满足特殊的 GStreamer 需求而进行扩展的，
 *   并且可以在 #GstObject 的基类 #GObject 中以相同的名称找到（例如 g_object_ref() 变成了 gst_object_ref()）。
 *
 *  由于 #GstObject 派生自 #GInitiallyUnowned，它还继承了浮动引用。
 *  请注意，诸如 gst_bin_add() 和 gst_element_add_pad() 等函数会获取浮动引用的所有权。
 *
 * 与 #GObject 实例相比，#GstObject 添加了一个名称属性。使用 gst_object_set_name() 和 gst_object_get_name() 函数来设置/获取对象的名称。
 *
 * ## 受控属性
 *
 * 受控属性提供了在流时间内调整 GObject 属性的轻量级方法。它通过使用时间戳值对进行元素属性的排队来工作。在运行时，元素不断拉取当前流时间的值更改。
 *
 * 在 #GstElement 中需要更改什么？
 * 非常少 - 仅需两个步骤即可使插件可控制！
 *
 *   * 通过 GST_PARAM_CONTROLLABLE 标记有意义的可以被控制的 gobject 属性 paramspecs。
 *
 *   * 在处理数据时（获取、链、循环函数）首先调用 gst_object_sync_values(element,timestamp)。
 *     这将使控制器使用基于时间戳的当前值更新其控制下的所有 GObject 属性。
 *
 * 在应用程序中需要做什么？同样需要更改的不多。
 *
 *   * 创建一个 #GstControlSource。
 *     csource = gst_interpolation_control_source_new ();
 *     g_object_set (csource, "mode", GST_INTERPOLATION_MODE_LINEAR, NULL);
 *
 *   * 将 #GstControlSource 附加到控制器上的属性。
 *     gst_object_add_control_binding (object, gst_direct_control_binding_new (object, "prop1", csource));
 *
 *   * 设置控制值
 *     gst_timed_value_control_source_set ((GstTimedValueControlSource *)csource,0 * GST_SECOND, value1);
 *     gst_timed_value_control_source_set ((GstTimedValueControlSource *)csource,1 * GST_SECOND, value2);
 *
 *   * 启动您的管道
 */

#include "gst_private.h"
#include "glib-compat-private.h"

#include "gstobject.h"
#include "gstclock.h"
#include "gstcontrolbinding.h"
#include "gstcontrolsource.h"
#include "gstinfo.h"
#include "gstparamspecs.h"
#include "gstutils.h"

#define DEBUG_REFCOUNT

/* 信号枚举 */
enum
{
  DEEP_NOTIFY,
  LAST_SIGNAL
};

/* 属性枚举 */
enum
{
  PROP_0,
  PROP_NAME,
  PROP_PARENT,
  PROP_LAST
};

/* 暂时没有被使用 */
enum
{
  SO_OBJECT_LOADED,
  SO_LAST_SIGNAL
};

/* maps type name quark => count */
static GData *object_name_counts = NULL;

G_LOCK_DEFINE_STATIC (object_name_mutex);

static void gst_object_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_object_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static void gst_object_dispatch_properties_changed (GObject * object,
    guint n_pspecs, GParamSpec ** pspecs);

static void gst_object_dispose (GObject * object);
static void gst_object_finalize (GObject * object);

static gboolean gst_object_set_name_default (GstObject * object);

static guint gst_object_signals[LAST_SIGNAL] = { 0 };

static GParamSpec *properties[PROP_LAST];

G_DEFINE_ABSTRACT_TYPE (GstObject, gst_object, G_TYPE_INITIALLY_UNOWNED);

static void
gst_object_constructed (GObject * object)
{
  GST_TRACER_OBJECT_CREATED (GST_OBJECT_CAST (object));

  ((GObjectClass *) gst_object_parent_class)->constructed (object);
}

static void
gst_object_class_init (GstObjectClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gst_object_set_property;
  gobject_class->get_property = gst_object_get_property;

  properties[PROP_NAME] =
      g_param_spec_string ("name", "Name", "The name of the object", NULL,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS |
      GST_PARAM_DOC_SHOW_DEFAULT);

  /**
   * GstObject:parent:
   *
   * 对象的父级。请注意，当更改 'parent' 属性时，由于锁定问题，我们不会发出 #GObject::notify 和 #GstObject::deep-notify 信号。
   * 在某些情况下，可以使用父级上的 #GstBin::element-added 或 #GstBin::element-removed 信号来实现类似的效果。
   */
  properties[PROP_PARENT] =
      g_param_spec_object ("parent", "Parent", "The parent of the object",
      GST_TYPE_OBJECT,
      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_DOC_SHOW_DEFAULT);

  g_object_class_install_properties (gobject_class, PROP_LAST, properties);

  /**
   * GstObject::deep-notify:
   * @gstobject: 一个 #GstObject
   * @prop_object: 发出信号的对象
   * @prop: 发生变化的属性
   *
   * deep-notify 信号用于通知属性变化。通常将其附加到顶层 bin，以接收该 bin 中包含的所有元素的通知。
   */
  gst_object_signals[DEEP_NOTIFY] =
      g_signal_new ("deep-notify", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_DETAILED |
      G_SIGNAL_NO_HOOKS, G_STRUCT_OFFSET (GstObjectClass, deep_notify), NULL,
      NULL, NULL, G_TYPE_NONE, 2, GST_TYPE_OBJECT, G_TYPE_PARAM);

  klass->path_string_separator = "/";

  /* 请参阅 gst_object_dispatch_properties_changed 中的注释 */
  gobject_class->dispatch_properties_changed
      = GST_DEBUG_FUNCPTR (gst_object_dispatch_properties_changed);

  gobject_class->constructed = gst_object_constructed;
  gobject_class->dispose = gst_object_dispose;
  gobject_class->finalize = gst_object_finalize;
}

static void
gst_object_init (GstObject * object)
{
  g_mutex_init (&object->lock);
  object->parent = NULL;
  object->name = NULL;
  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p new", object);

  object->flags = 0;

  object->control_rate = 100 * GST_MSECOND;
  object->last_sync = GST_CLOCK_TIME_NONE;
}

/**
 * gst_object_ref:
 * @object: （类型 Gst.Object）：要引用的 #GstObject
 *
 * 增加 @object 上的引用计数。此函数不会锁定 @object，因为它依赖于原子引用计数。
 *
 * 此对象返回输入参数，以便更轻松地编写类似以下结构的代码：
 *  result = gst_object_ref (object->parent);
 *
 * 返回：（传输完整）（类型 Gst.Object）：指向 @object 的指针
 */
gpointer
gst_object_ref (gpointer object)
{
  g_return_val_if_fail (object != NULL, NULL);

  GST_TRACER_OBJECT_REFFED (object, ((GObject *) object)->ref_count + 1);
#ifdef DEBUG_REFCOUNT
  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p ref %d->%d", object,
      ((GObject *) object)->ref_count, ((GObject *) object)->ref_count + 1);
#endif
  g_object_ref (object);

  return object;
}

/**
 * gst_object_unref:
 * @object: （类型 Gst.Object）：要取消引用的 #GstObject
 *
 * 减少 @object 上的引用计数。如果引用计数减至零，则销毁 @object。此函数不会锁定 @object，因为它依赖于原子引用计数。
 *
 * 不应在持有锁的情况下调用取消引用方法，因为这可能导致销毁函数死锁。
 */
void
gst_object_unref (gpointer object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (((GObject *) object)->ref_count > 0);

  GST_TRACER_OBJECT_UNREFFED (object, ((GObject *) object)->ref_count - 1);
#ifdef DEBUG_REFCOUNT
  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p unref %d->%d", object,
      ((GObject *) object)->ref_count, ((GObject *) object)->ref_count - 1);
#endif
  g_object_unref (object);
}

/**
 * gst_object_ref_sink: (跳过)
 * @object: 要unref的 #GstObject
 *
 * 增加 @object 的引用计数，并在可能的情况下移除浮动引用（如果 @object 具有浮动引用）。
 *
 * 换句话说，如果对象是浮动的，那么此调用将“拥有”浮动引用，通过清除浮动标志而不更改引用计数将其转换为正常引用。
 * 如果对象不是浮动的，那么此调用将添加一个新的正常引用，将引用计数增加一。
 *
 * 有关“浮动引用”的更多背景信息，请参阅 #GObject 文档。
 */
gpointer
gst_object_ref_sink (gpointer object)
{
  g_return_val_if_fail (object != NULL, NULL);

#ifdef DEBUG_REFCOUNT
  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p ref_sink %d->%d",
      object, ((GObject *) object)->ref_count,
      ((GObject *) object)->ref_count + 1);
#endif
  return g_object_ref_sink (object);
}

/**
 * gst_clear_object: (跳过)
 * @object_ptr: 指向 #GstObject 引用的指针
 *
 * 清除对 #GstObject 的引用。
 *
 * @object_ptr 不能为空。
 *
 * 如果引用为 %NULL，则此函数不执行任何操作。否则，使用 gst_object_unref() 减少对象的引用计数，并将指针设置为 %NULL。
 *
 * 还包含一个允许在没有指针转换的情况下使用此函数的宏。
 *
 * 自版本：1.16
 **/
#undef gst_clear_object
void
gst_clear_object (GstObject ** object_ptr)
{
  g_clear_pointer (object_ptr, gst_object_unref);
}

/**
 * gst_object_replace:
 * @oldobj: (inout) (transfer full) (nullable): 指向要替换的 #GstObject 的指针
 * @newobj: (transfer none) (nullable): 新的 #GstObject
 *
 * 原子地修改指针以指向一个新对象。减少 @oldobj 的引用计数，并增加 @newobj 的引用计数。
 *
 * @newobj 和 @oldobj 指向的值都可以为 %NULL。
 *
 * 返回：如果 @newobj 与 @oldobj 不同，则为 %TRUE
 */
gboolean
gst_object_replace (GstObject ** oldobj, GstObject * newobj)
{
  GstObject *oldptr;

  g_return_val_if_fail (oldobj != NULL, FALSE);

#ifdef DEBUG_REFCOUNT
  GST_CAT_TRACE (GST_CAT_REFCOUNTING, "replace %p %s (%d) with %p %s (%d)",
      *oldobj, *oldobj ? GST_STR_NULL (GST_OBJECT_NAME (*oldobj)) : "(NONE)",
      *oldobj ? G_OBJECT (*oldobj)->ref_count : 0,
      newobj, newobj ? GST_STR_NULL (GST_OBJECT_NAME (newobj)) : "(NONE)",
      newobj ? G_OBJECT (newobj)->ref_count : 0);
#endif

  oldptr = (GstObject *) g_atomic_pointer_get ((gpointer *) oldobj);

  if (G_UNLIKELY (oldptr == newobj))
    return FALSE;

  if (newobj)
    gst_object_ref (newobj);

  while (G_UNLIKELY (!g_atomic_pointer_compare_and_exchange ((gpointer *)
              oldobj, (gpointer) oldptr, newobj))) {
    oldptr = g_atomic_pointer_get ((gpointer *) oldobj);
    if (G_UNLIKELY (oldptr == newobj))
      break;
  }

  if (oldptr)
    gst_object_unref (oldptr);

  return oldptr != newobj;
}

/* dispose 在对象需要释放到其他对象的所有链接时调用 */
static void
gst_object_dispose (GObject * object)
{
  GstObject *self = (GstObject *) object;
  GstObject *parent;

  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p dispose", object);

  GST_OBJECT_LOCK (object);
  if ((parent = GST_OBJECT_PARENT (object)))
    goto have_parent;
  GST_OBJECT_PARENT (object) = NULL;
  GST_OBJECT_UNLOCK (object);

  if (self->control_bindings) {
    GList *node;

    for (node = self->control_bindings; node; node = g_list_next (node)) {
      gst_object_unparent (node->data);
    }
    g_list_free (self->control_bindings);
    self->control_bindings = NULL;
  }

  ((GObjectClass *) gst_object_parent_class)->dispose (object);

  return;

  /* ERRORS */
have_parent:
  {
    g_critical ("\nTrying to dispose object \"%s\", but it still has a "
        "parent \"%s\".\nYou need to let the parent manage the "
        "object instead of unreffing the object directly.\n",
        GST_OBJECT_NAME (object), GST_OBJECT_NAME (parent));
    GST_OBJECT_UNLOCK (object);
    /* ref the object again to revive it in this error case */
    gst_object_ref (object);
    return;
  }
}

/* finalize 在对象需要释放其资源时调用 */
static void
gst_object_finalize (GObject * object)
{
  GstObject *gstobject = GST_OBJECT_CAST (object);

  GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "%p finalize", object);

  g_signal_handlers_destroy (object);

  g_free (gstobject->name);
  g_mutex_clear (&gstobject->lock);

  GST_TRACER_OBJECT_DESTROYED (gstobject);

  ((GObjectClass *) gst_object_parent_class)->finalize (object);
}

/**
 * 更改 GstObject 的 GObject 属性将导致对象本身发出 "deep-notify" 信号，以及每个父对象都会发出该信号。
 * 这样，应用程序可以连接一个监听器到顶层 bin，以捕获所有包含元素的属性更改通知。
 * MT（多线程）安全。
 */
static void
gst_object_dispatch_properties_changed (GObject * object,
    guint n_pspecs, GParamSpec ** pspecs)
{
  GstObject *gst_object, *parent, *old_parent;
  guint i;
#ifndef GST_DISABLE_GST_DEBUG
  gchar *name = NULL;
  const gchar *debug_name;
#endif

  /* do the standard dispatching */
  /* GstObject的父类调用dispatch_properties_changed，也就是GInitiallyUnowned对象 */
  ((GObjectClass *)
      gst_object_parent_class)->dispatch_properties_changed (object, n_pspecs,
      pspecs);

  gst_object = GST_OBJECT_CAST (object);
#ifndef GST_DISABLE_GST_DEBUG
  if (G_UNLIKELY (_gst_debug_min >= GST_LEVEL_LOG)) {
    name = gst_object_get_name (gst_object);
    debug_name = GST_STR_NULL (name);
  } else
    debug_name = "";
#endif

  /* 一层一层获取链接的parent发出该信号 */
  parent = gst_object_get_parent (gst_object);
  while (parent) {
    /* n_pspecs 表示有多少个属性发生了变化 */
    for (i = 0; i < n_pspecs; i++) {
      GST_CAT_LOG_OBJECT (GST_CAT_PROPERTIES, parent,
          "deep notification from %s (%s)", debug_name, pspecs[i]->name);

      g_signal_emit (parent, gst_object_signals[DEEP_NOTIFY],
          g_quark_from_string (pspecs[i]->name), gst_object, pspecs[i]);
    }

    old_parent = parent;
    parent = gst_object_get_parent (old_parent);
    gst_object_unref (old_parent);
  }
#ifndef GST_DISABLE_GST_DEBUG
  g_free (name);
#endif
}

/**
 * gst_object_default_deep_notify:
 * @object: 发出通知的 #GObject。
 * @orig: 发起通知的 #GstObject。
 * @pspec: 属性的 #GParamSpec。
 * @excluded_props: （数组以零结尾=1）（元素类型 gchar*）（可为 NULL）：要排除的一组用户指定属性，或者为 %NULL 以显示所有更改。
 *
 * 用于对象的默认 deep_notify 信号回调。用户数据应包含指向应从通知中排除的字符串数组的指针。默认处理程序将使用 g_print 打印属性的新值。
 *
 * MT 安全。此函数获取并释放 @object 的 LOCK 以获取其路径字符串。
 */
void
gst_object_default_deep_notify (GObject * object, GstObject * orig,
    GParamSpec * pspec, gchar ** excluded_props)
{
  GValue value = { 0, };        /* the important thing is that value.type = 0 */
  gchar *str = NULL;
  gchar *name = NULL;

  if (pspec->flags & G_PARAM_READABLE) {
    /* let's not print these out for excluded properties... */
    while (excluded_props != NULL && *excluded_props != NULL) {
      if (strcmp (pspec->name, *excluded_props) == 0)
        return;
      excluded_props++;
    }
    g_value_init (&value, pspec->value_type);
    g_object_get_property (G_OBJECT (orig), pspec->name, &value);

    if (G_VALUE_HOLDS_STRING (&value))
      str = g_value_dup_string (&value);
    else
      str = gst_value_serialize (&value);
    name = gst_object_get_path_string (orig);
    g_print ("%s: %s = %s\n", name, pspec->name, str);
    g_free (name);
    g_free (str);
    g_value_unset (&value);
  } else {
    name = gst_object_get_path_string (orig);
    g_warning ("Parameter %s not readable in %s.", pspec->name, name);
    g_free (name);
  }
}

static gboolean
gst_object_set_name_default (GstObject * object)
{
  const gchar *type_name;
  gint count;
  gchar *name;
  GQuark q;
  guint i, l;

  /* to ensure guaranteed uniqueness across threads, only one thread
   * may ever assign a name */
  G_LOCK (object_name_mutex);

  if (!object_name_counts) {
    g_datalist_init (&object_name_counts);
  }

  q = g_type_qname (G_OBJECT_TYPE (object));
  count = GPOINTER_TO_INT (g_datalist_id_get_data (&object_name_counts, q));
  g_datalist_id_set_data (&object_name_counts, q, GINT_TO_POINTER (count + 1));

  G_UNLOCK (object_name_mutex);

  /* GstFooSink -> foosink<N> */
  type_name = g_quark_to_string (q);
  if (strncmp (type_name, "Gst", 3) == 0)
    type_name += 3;
  /* give the 20th "queue" element and the first "queue2" different names */
  l = strlen (type_name);
  if (l > 0 && g_ascii_isdigit (type_name[l - 1])) {
    name = g_strdup_printf ("%s-%d", type_name, count);
  } else {
    name = g_strdup_printf ("%s%d", type_name, count);
  }

  l = strlen (name);
  for (i = 0; i < l; i++)
    name[i] = g_ascii_tolower (name[i]);

  GST_OBJECT_LOCK (object);
  if (G_UNLIKELY (object->parent != NULL))
    goto had_parent;

  g_free (object->name);
  object->name = name;

  GST_OBJECT_UNLOCK (object);

  return TRUE;

had_parent:
  {
    g_free (name);
    GST_WARNING ("parented objects can't be renamed");
    GST_OBJECT_UNLOCK (object);
    return FALSE;
  }
}

static gboolean
gst_object_set_name_intern (GstObject * object, const gchar * name)
{
  gboolean result;

  GST_OBJECT_LOCK (object);

  /* parented objects cannot be renamed */
  if (G_UNLIKELY (object->parent != NULL))
    goto had_parent;

  if (name != NULL) {
    g_free (object->name);
    object->name = g_strdup (name);
    GST_OBJECT_UNLOCK (object);
    result = TRUE;
  } else {
    GST_OBJECT_UNLOCK (object);
    result = gst_object_set_name_default (object);
  }

  return result;

  /* error */
had_parent:
  {
    GST_WARNING ("parented objects can't be renamed");
    GST_OBJECT_UNLOCK (object);
    return FALSE;
  }
}

/**
 * gst_object_set_name:
 * @object: 一个 #GstObject
 * @name: （可为 NULL）：对象的新名称
 *
 * 设置 @object 的名称，或者为 @object 分配一个确保唯一的名称（如果 @name 为 %NULL）。
 * 此函数复制提供的名称，因此调用方保留其发送的名称的所有权。
 *
 * 返回：如果名称可以设置，则为 %TRUE。由于具有父级的对象无法重命名，在这些情况下此函数返回 %FALSE。
 *
 * MT（多线程）安全。此函数获取并释放 @object 的 LOCK。
 */
gboolean
gst_object_set_name (GstObject * object, const gchar * name)
{
  gboolean result;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);

  if ((result = gst_object_set_name_intern (object, name)))
    g_object_notify_by_pspec (G_OBJECT (object), properties[PROP_NAME]);
  return result;
}

/**
 * gst_object_get_name:
 * @object: 一个 #GstObject
 *
 * 返回 @object 名称的副本。
 * 调用方在使用后应该使用 g_free() 释放返回值。
 * 对于无名称的对象，这将返回 %NULL，您也可以安全地使用 g_free()。
 *
 * Free 函数：g_free
 *
 * 返回：（传输完整）（可为 NULL）：@object 的名称。在使用后请使用 g_free()。
 *
 * MT（多线程）安全。此函数获取并释放 @object 的 LOCK。
 */
gchar *
gst_object_get_name (GstObject * object)
{
  gchar *result = NULL;

  g_return_val_if_fail (GST_IS_OBJECT (object), NULL);

  GST_OBJECT_LOCK (object);
  result = g_strdup (object->name);
  GST_OBJECT_UNLOCK (object);

  return result;
}

/**
 * gst_object_set_parent:
 * @object: （传递浮动引用）：一个 #GstObject
 * @parent: 对象的新父级
 *
 * 将 @object 的父级设置为 @parent。对象的引用计数将被增加，并且任何浮动引用都将被移除（参见 gst_object_ref_sink()）。
 *
 * 返回：如果可以设置 @parent，则为 %TRUE，或者当 @object 已经有一个父级或者 @object 和 @parent 是相同的时候为 %FALSE。
 *
 * MT（多线程）安全。获取并释放 @object 的 LOCK。
 */
gboolean
gst_object_set_parent (GstObject * object, GstObject * parent)
{
  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (GST_IS_OBJECT (parent), FALSE);
  g_return_val_if_fail (object != parent, FALSE);

  GST_CAT_DEBUG_OBJECT (GST_CAT_REFCOUNTING, object,
      "set parent (ref and sink)");

  GST_OBJECT_LOCK (object);
  if (G_UNLIKELY (object->parent != NULL))
    goto had_parent;

  object->parent = parent;
  gst_object_ref_sink (object);
  GST_OBJECT_UNLOCK (object);

  /* FIXME-2.0: 这不起作用，deep notify 会从父对象中获取锁，
     在父对象在调用此函数时持有锁时发生死锁（例如 _element_add_pad()），
     我们需要使用 GRecMutex 来锁定父对象。 */
  /* g_object_notify_by_pspec ((GObject *)object, properties[PROP_PARENT]); */

  return TRUE;

  /* ERROR handling */
had_parent:
  {
    GST_CAT_DEBUG_OBJECT (GST_CAT_REFCOUNTING, object,
        "set parent failed, object already had a parent");
    gst_object_ref_sink (object);
    gst_object_unref (object);
    GST_OBJECT_UNLOCK (object);
    return FALSE;
  }
}

/**
 * gst_object_get_parent:
 * @object: 一个 #GstObject
 *
 * 返回 @object 的父级。此函数增加父对象的引用计数，因此在使用后应调用 gst_object_unref()。
 *
 * 返回：（传输完整）（可为 NULL）：@object 的父级，如果 @object 没有父级，则为 %NULL。使用后请取消引用。
 *
 * MT（多线程）安全。获取并释放 @object 的 LOCK。
 */
GstObject *
gst_object_get_parent (GstObject * object)
{
  GstObject *result = NULL;

  g_return_val_if_fail (GST_IS_OBJECT (object), NULL);

  GST_OBJECT_LOCK (object);
  result = object->parent;
  if (G_LIKELY (result))
    gst_object_ref (result);
  GST_OBJECT_UNLOCK (object);

  return result;
}

/**
 * gst_object_unparent:
 * @object: 要取消父级关联的 #GstObject
 *
 * 清除 @object 的父级，移除关联的引用。此函数减少 @object 的引用计数。
 *
 * MT（多线程）安全。获取并释放 @object 的锁。
 */
void
gst_object_unparent (GstObject * object)
{
  GstObject *parent;

  g_return_if_fail (GST_IS_OBJECT (object));

  GST_OBJECT_LOCK (object);
  parent = object->parent;

  if (G_LIKELY (parent != NULL)) {
    GST_CAT_TRACE_OBJECT (GST_CAT_REFCOUNTING, object, "unparent");
    object->parent = NULL;
    GST_OBJECT_UNLOCK (object);

    /* g_object_notify_by_pspec ((GObject *)object, properties[PROP_PARENT]); */

    gst_object_unref (object);
  } else {
    GST_OBJECT_UNLOCK (object);
  }
}

/**
 * gst_object_has_as_parent:
 * @object: 要检查的 #GstObject
 * @parent: 要作为父级检查的 #GstObject
 *
 * 检查 @parent 是否是 @object 的父级。
 * 例如，一个 #GstElement 可以检查它是否拥有给定的 #GstPad。
 *
 * 返回：如果 @object 或 @parent 任一为空，则为 %FALSE。如果 @parent 是 @object 的父级，则为 %TRUE。否则为 %FALSE。
 *
 * MT（多线程）安全。获取并释放 @object 的锁。
 * 自版本：1.6
 */
gboolean
gst_object_has_as_parent (GstObject * object, GstObject * parent)
{
  gboolean result = FALSE;

  if (G_LIKELY (GST_IS_OBJECT (object) && GST_IS_OBJECT (parent))) {
    GST_OBJECT_LOCK (object);
    result = GST_OBJECT_PARENT (object) == parent;
    GST_OBJECT_UNLOCK (object);
  }

  return result;
}

/**
 * gst_object_has_as_ancestor:
 * @object: 要检查的 #GstObject
 * @ancestor: 要作为祖先检查的 #GstObject
 *
 * 检查 @object 是否在层次结构中的某个地方具有祖先 @ancestor。例如，可以检查 #GstElement 是否在 #GstPipeline 中。
 *
 * 返回：如果 @ancestor 是 @object 的祖先，则为 %TRUE。
 *
 * MT（多线程）安全。获取并释放 @object 的锁。
 */
gboolean
gst_object_has_as_ancestor (GstObject * object, GstObject * ancestor)
{
  GstObject *parent, *tmp;

  if (!ancestor || !object)
    return FALSE;

  parent = gst_object_ref (object);
  do {
    if (parent == ancestor) {
      gst_object_unref (parent);
      return TRUE;
    }

    tmp = gst_object_get_parent (parent);
    gst_object_unref (parent);
    parent = tmp;
  } while (parent);

  return FALSE;
}

/**
 * gst_object_has_ancestor:
 * @object: 要检查的 #GstObject
 * @ancestor: 要作为祖先检查的 #GstObject
 *
 * 检查 @object 是否在层次结构中的某个地方具有祖先 @ancestor。例如，可以检查 #GstElement 是否在 #GstPipeline 中。
 *
 * 返回：如果 @ancestor 是 @object 的祖先，则为 %TRUE。
 *
 * 弃用：请改用 gst_object_has_as_ancestor()。
 *
 * MT（多线程）安全。获取并释放 @object 的锁。
 */
#ifndef GST_REMOVE_DEPRECATED
gboolean
gst_object_has_ancestor (GstObject * object, GstObject * ancestor)
{
  return gst_object_has_as_ancestor (object, ancestor);
}
#endif

/**
 * gst_object_check_uniqueness:
 * @list: （传输无）（元素类型为 Gst.Object）：要检查的 #GstObject 列表
 * @name: 要搜索的名称
 *
 * 检查 @list 中是否存在任何名称为 @name 的对象。此函数不执行任何类型的锁定。
 * 您可能希望使用列表所有者的锁保护提供的列表。此函数将锁定列表中的每个 #GstObject 以比较名称，因此在传递具有已锁定对象的列表时要小心。
 *
 * 返回：如果在 @list 中没有名为 @name 的 #GstObject，则为 %TRUE，如果存在则为 %FALSE。
 *
 * MT（多线程）安全。获取并释放列表中每个对象的 LOCK。
 */
gboolean
gst_object_check_uniqueness (GList * list, const gchar * name)
{
  gboolean result = TRUE;

  g_return_val_if_fail (name != NULL, FALSE);

  for (; list; list = g_list_next (list)) {
    GstObject *child;
    gboolean eq;

    child = GST_OBJECT_CAST (list->data);

    GST_OBJECT_LOCK (child);
    eq = strcmp (GST_OBJECT_NAME (child), name) == 0;
    GST_OBJECT_UNLOCK (child);

    if (G_UNLIKELY (eq)) {
      result = FALSE;
      break;
    }
  }
  return result;
}


static void
gst_object_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstObject *gstobject;

  gstobject = GST_OBJECT_CAST (object);

  switch (prop_id) {
    case PROP_NAME:
      gst_object_set_name_intern (gstobject, g_value_get_string (value));
      break;
    case PROP_PARENT:
      gst_object_set_parent (gstobject, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_object_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstObject *gstobject;

  gstobject = GST_OBJECT_CAST (object);

  switch (prop_id) {
    case PROP_NAME:
      g_value_take_string (value, gst_object_get_name (gstobject));
      break;
    case PROP_PARENT:
      g_value_take_object (value, gst_object_get_parent (gstobject));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/**
 * gst_object_get_path_string:
 * @object: 一个 #GstObject
 *
 * 生成一个描述 @object 在对象层次结构中路径的字符串。仅用于调试（或被用于调试）。
 *
 * Free 函数：g_free
 *
 * 返回：（传输完整）：描述 @object 路径的字符串。在使用后，您必须使用 g_free() 释放该字符串。
 *
 * MT（多线程）安全。获取并释放层次结构中所有对象的 #GstObject 的 LOCK。
 */
gchar *
gst_object_get_path_string (GstObject * object)
{
  GSList *parentage;
  GSList *parents;
  void *parent;
  gchar *prevpath, *path;
  const gchar *typename;
  gchar *component;
  const gchar *separator;

  /* ref object before adding to list */
  gst_object_ref (object);
  parentage = g_slist_prepend (NULL, object);

  path = g_strdup ("");

  /* first walk the object hierarchy to build a list of the parents,
   * be careful here with refcounting. */
  do {
    if (GST_IS_OBJECT (object)) {
      parent = gst_object_get_parent (object);
      /* add parents to list, refcount remains increased while
       * we handle the object */
      if (parent)
        parentage = g_slist_prepend (parentage, parent);
    } else {
      break;
    }
    object = parent;
  } while (object != NULL);

  /* then walk the parent list and print them out. we need to
   * decrease the refcounting on each element after we handled
   * it. */
  for (parents = parentage; parents; parents = g_slist_next (parents)) {
    if (G_IS_OBJECT (parents->data)) {
      typename = G_OBJECT_TYPE_NAME (parents->data);
    } else {
      typename = NULL;
    }
    if (GST_IS_OBJECT (parents->data)) {
      GstObject *item = GST_OBJECT_CAST (parents->data);
      GstObjectClass *oclass = GST_OBJECT_GET_CLASS (item);
      gchar *objname = gst_object_get_name (item);

      component = g_strdup_printf ("%s:%s", typename, objname);
      separator = oclass->path_string_separator;
      /* and unref now */
      gst_object_unref (item);
      g_free (objname);
    } else {
      if (typename) {
        component = g_strdup_printf ("%s:%p", typename, parents->data);
      } else {
        component = g_strdup_printf ("%p", parents->data);
      }
      separator = "/";
    }

    prevpath = path;
    path = g_strjoin (separator, prevpath, component, NULL);
    g_free (prevpath);
    g_free (component);
  }

  g_slist_free (parentage);

  return path;
}

/* controller helper functions */

/*
 * gst_object_find_control_binding:
 * @self: 要在其中搜索属性的 GObject
 * @name: 要查找的 GObject 属性名称
 *
 * 搜索受控制的属性列表。
 *
 * 返回：（可为 NULL）：一个 #GstControlBinding，如果未控制该属性，则为 %NULL。
 */
static GstControlBinding *
gst_object_find_control_binding (GstObject * self, const gchar * name)
{
  GstControlBinding *binding;
  GList *node;

  for (node = self->control_bindings; node; node = g_list_next (node)) {
    binding = node->data;
    /* FIXME: eventually use GQuark to speed it up */
    if (!strcmp (binding->name, name)) {
      GST_DEBUG_OBJECT (self, "found control binding for property '%s'", name);
      return binding;
    }
  }
  GST_DEBUG_OBJECT (self, "controller does not manage property '%s'", name);

  return NULL;
}

/* controller functions */

/**
 * gst_object_suggest_next_sync:
 * @object: 具有受控属性的对象
 *
 * 返回建议的时间戳，用于在获取最佳控制器结果时分割缓冲区。
 *
 * 返回：返回建议的时间戳，如果未设置控制速率，则为 %GST_CLOCK_TIME_NONE。
 */
GstClockTime
gst_object_suggest_next_sync (GstObject * object)
{
  GstClockTime ret;

  g_return_val_if_fail (GST_IS_OBJECT (object), GST_CLOCK_TIME_NONE);
  g_return_val_if_fail (object->control_rate != GST_CLOCK_TIME_NONE,
      GST_CLOCK_TIME_NONE);

  GST_OBJECT_LOCK (object);

  /* TODO: Implement more logic, depending on interpolation mode and control
   * points
   * FIXME: we need playback direction
   */
  ret = object->last_sync + object->control_rate;

  GST_OBJECT_UNLOCK (object);

  return ret;
}

/**
 * gst_object_sync_values:
 * @object: 具有受控属性的对象
 * @timestamp: 应该处理的时间
 *
 * 根据（可能）处理它们的 #GstControlSources 和给定的时间戳设置对象的属性。
 *
 * 如果此函数失败，最有可能是应用程序开发人员的错误。很可能是控制源未正确设置。
 *
 * 返回：如果控制器值可以应用于对象属性，则为 %TRUE，否则为 %FALSE。
 */
gboolean
gst_object_sync_values (GstObject * object, GstClockTime timestamp)
{
  GList *node;
  gboolean ret = TRUE;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (timestamp), FALSE);

  GST_LOG_OBJECT (object, "sync_values");
  if (!object->control_bindings)
    return TRUE;

  /* FIXME: this deadlocks */
  /* GST_OBJECT_LOCK (object); */
  g_object_freeze_notify ((GObject *) object);
  for (node = object->control_bindings; node; node = g_list_next (node)) {
    ret &= gst_control_binding_sync_values ((GstControlBinding *) node->data,
        object, timestamp, object->last_sync);
  }
  object->last_sync = timestamp;
  g_object_thaw_notify ((GObject *) object);
  /* GST_OBJECT_UNLOCK (object); */

  return ret;
}


/**
 * gst_object_has_active_control_bindings:
 * @object: 具有受控属性的对象
 *
 * 检查 @object 是否具有活动的受控属性。
 *
 * 返回：如果对象具有活动的受控属性，则为 %TRUE。
 */
gboolean
gst_object_has_active_control_bindings (GstObject * object)
{
  gboolean res = FALSE;
  GList *node;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);

  GST_OBJECT_LOCK (object);
  for (node = object->control_bindings; node; node = g_list_next (node)) {
    res |= !gst_control_binding_is_disabled ((GstControlBinding *) node->data);
  }
  GST_OBJECT_UNLOCK (object);
  return res;
}

/**
 * gst_object_set_control_bindings_disabled:
 * @object: 具有受控属性的对象
 * @disabled: 一个布尔值，指定是否禁用控制器。
 *
 * 此函数用于在一段时间内禁用 @object 的所有受控属性，即 gst_object_sync_values() 将不执行任何操作。
 */
void
gst_object_set_control_bindings_disabled (GstObject * object, gboolean disabled)
{
  GList *node;

  g_return_if_fail (GST_IS_OBJECT (object));

  GST_OBJECT_LOCK (object);
  for (node = object->control_bindings; node; node = g_list_next (node)) {
    gst_control_binding_set_disabled ((GstControlBinding *) node->data,
        disabled);
  }
  GST_OBJECT_UNLOCK (object);
}

/**
 * gst_object_set_control_binding_disabled:
 * @object: 具有受控属性的对象
 * @property_name: 要禁用的属性
 * @disabled: 一个布尔值，指定是否禁用控制器。
 *
 * 此函数用于在一段时间内禁用属性上的控制绑定，即 gst_object_sync_values() 将不对该属性执行任何操作。
 */
void
gst_object_set_control_binding_disabled (GstObject * object,
    const gchar * property_name, gboolean disabled)
{
  GstControlBinding *binding;

  g_return_if_fail (GST_IS_OBJECT (object));
  g_return_if_fail (property_name);

  GST_OBJECT_LOCK (object);
  if ((binding = gst_object_find_control_binding (object, property_name))) {
    gst_control_binding_set_disabled (binding, disabled);
  }
  GST_OBJECT_UNLOCK (object);
}


/**
 * gst_object_add_control_binding:
 * @object: 控制器对象
 * @binding: （传递浮动引用）：应该使用的 #GstControlBinding
 *
 * 将 #GstControlBinding 附加到对象。如果已经存在用于此属性的 #GstControlBinding，则将其替换。
 *
 * 对象的引用计数将被增加，并且将删除任何浮动引用（参见 gst_object_ref_sink()）。
 *
 * 返回：如果给定的 @binding 尚未为此对象设置或已为不适合的属性设置，则为 %FALSE；否则为 %TRUE。
 */
gboolean
gst_object_add_control_binding (GstObject * object, GstControlBinding * binding)
{
  GstControlBinding *old;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (GST_IS_CONTROL_BINDING (binding), FALSE);
  g_return_val_if_fail (binding->pspec, FALSE);

  GST_OBJECT_LOCK (object);
  if ((old = gst_object_find_control_binding (object, binding->name))) {
    GST_DEBUG_OBJECT (object, "controlled property %s removed", old->name);
    object->control_bindings = g_list_remove (object->control_bindings, old);
    gst_object_unparent (GST_OBJECT_CAST (old));
  }
  object->control_bindings = g_list_prepend (object->control_bindings, binding);
  gst_object_set_parent (GST_OBJECT_CAST (binding), object);
  GST_DEBUG_OBJECT (object, "controlled property %s added", binding->name);
  GST_OBJECT_UNLOCK (object);

  return TRUE;
}

/**
 * gst_object_get_control_binding:
 * @object: 对象
 * @property_name: 属性的名称
 *
 * 获取属性的相应 #GstControlBinding。在使用后应再次取消引用。
 *
 * 返回：（传输完整）（可为 NULL）：@property_name 的 #GstControlBinding，如果属性未受控制则为 %NULL。
 */
GstControlBinding *
gst_object_get_control_binding (GstObject * object, const gchar * property_name)
{
  GstControlBinding *binding;

  g_return_val_if_fail (GST_IS_OBJECT (object), NULL);
  g_return_val_if_fail (property_name, NULL);

  GST_OBJECT_LOCK (object);
  if ((binding = gst_object_find_control_binding (object, property_name))) {
    gst_object_ref (binding);
  }
  GST_OBJECT_UNLOCK (object);

  return binding;
}

/**
 * gst_object_remove_control_binding:
 * @object: 对象
 * @binding: 绑定
 *
 * 删除相应的 #GstControlBinding。如果它是绑定的最后一个引用，它将被处理。
 *
 * 返回：如果可以删除绑定，则为 %TRUE。
 */
gboolean
gst_object_remove_control_binding (GstObject * object,
    GstControlBinding * binding)
{
  GList *node;
  gboolean ret = FALSE;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (GST_IS_CONTROL_BINDING (binding), FALSE);

  GST_OBJECT_LOCK (object);
  if ((node = g_list_find (object->control_bindings, binding))) {
    GST_DEBUG_OBJECT (object, "controlled property %s removed", binding->name);
    object->control_bindings =
        g_list_delete_link (object->control_bindings, node);
    gst_object_unparent (GST_OBJECT_CAST (binding));
    ret = TRUE;
  }
  GST_OBJECT_UNLOCK (object);

  return ret;
}

/**
 * gst_object_get_value:
 * @object: 具有受控属性的对象
 * @property_name: 要获取的属性的名称
 * @timestamp: 应从中读取控制更改的时间
 *
 * 获取请求时间的给定受控属性的值。
 *
 * 返回：（传输完整）（可为 NULL）：给定时间属性的 GValue，如果该属性未受控制则为 %NULL。
 */
GValue *
gst_object_get_value (GstObject * object, const gchar * property_name,
    GstClockTime timestamp)
{
  GstControlBinding *binding;
  GValue *val = NULL;

  g_return_val_if_fail (GST_IS_OBJECT (object), NULL);
  g_return_val_if_fail (property_name, NULL);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (timestamp), NULL);

  GST_OBJECT_LOCK (object);
  if ((binding = gst_object_find_control_binding (object, property_name))) {
    val = gst_control_binding_get_value (binding, timestamp);
  }
  GST_OBJECT_UNLOCK (object);

  return val;
}

/**
 * gst_object_get_value_array: (跳过)
 * @object: 具有受控属性的对象
 * @property_name: 要获取的属性的名称
 * @timestamp: 应该处理的时间
 * @interval: 后续值之间的时间间隔
 * @n_values: 值的数量
 * @values: （数组长度=n_values）：用于放置控制值的数组
 *
 * 获取给定受控属性的一系列值，从请求的时间开始。数组 @values 需要具有足够的空间，以容纳 @n_values 个与对象属性类型相同的值。
 *
 * 如果想要绘制控制曲线的图形或逐个样本应用控制曲线，此函数很有用。
 *
 * 值已解包并准备好使用。类似的函数 gst_object_get_g_value_array() 将数组作为 #GValues 返回，更适用于绑定。
 *
 * 返回：如果可以填充给定的数组，则为 %TRUE，否则为 %FALSE。
 */
gboolean
gst_object_get_value_array (GstObject * object, const gchar * property_name,
    GstClockTime timestamp, GstClockTime interval, guint n_values,
    gpointer values)
{
  gboolean res = FALSE;
  GstControlBinding *binding;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (property_name, FALSE);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (timestamp), FALSE);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (interval), FALSE);
  g_return_val_if_fail (values, FALSE);

  GST_OBJECT_LOCK (object);
  if ((binding = gst_object_find_control_binding (object, property_name))) {
    res = gst_control_binding_get_value_array (binding, timestamp, interval,
        n_values, values);
  }
  GST_OBJECT_UNLOCK (object);
  return res;
}

/**
 * gst_object_get_g_value_array:
 * @object: 具有受控属性的对象
 * @property_name: 要获取的属性的名称
 * @timestamp: 应该处理的时间
 * @interval: 后续值之间的时间间隔
 * @n_values: 值的数量
 * @values: （数组长度=n_values）：用于放置控制值的数组
 *
 * 获取给定受控属性的一系列 #GValues，从请求的时间开始。数组 @values 需要具有足够的空间，以容纳 @n_values 个 #GValue。
 *
 * 如果想要绘制控制曲线的图形或逐个样本应用控制曲线，此函数很有用。
 *
 * 返回：如果可以填充给定的数组，则为 %TRUE，否则为 %FALSE。
 */
gboolean
gst_object_get_g_value_array (GstObject * object, const gchar * property_name,
    GstClockTime timestamp, GstClockTime interval, guint n_values,
    GValue * values)
{
  gboolean res = FALSE;
  GstControlBinding *binding;

  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (property_name, FALSE);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (timestamp), FALSE);
  g_return_val_if_fail (GST_CLOCK_TIME_IS_VALID (interval), FALSE);
  g_return_val_if_fail (values, FALSE);

  GST_OBJECT_LOCK (object);
  if ((binding = gst_object_find_control_binding (object, property_name))) {
    res = gst_control_binding_get_g_value_array (binding, timestamp, interval,
        n_values, values);
  }
  GST_OBJECT_UNLOCK (object);
  return res;
}


/**
 * gst_object_get_control_rate:
 * @object: 具有受控属性的对象
 *
 * 获取此 @object 的控制速率。音频处理的 #GstElement 对象将使用此速率来细分其处理循环，并在其中调用 gst_object_sync_values()。处理段的长度应该达到 @control-rate 纳秒。
 *
 * 如果 @object 不受属性控制，则将返回 %GST_CLOCK_TIME_NONE。这允许元素避免细分。
 *
 * 如果元素处于 %GST_STATE_PAUSED 或 %GST_STATE_PLAYING，不希望控制速率发生更改。
 *
 * 返回：以纳秒为单位的控制速率
 */
GstClockTime
gst_object_get_control_rate (GstObject * object)
{
  g_return_val_if_fail (GST_IS_OBJECT (object), FALSE);

  return object->control_rate;
}

/**
 * gst_object_set_control_rate:
 * @object: 具有受控属性的对象
 * @control_rate: 以纳秒为单位的新控制速率。
 *
 * 更改此 @object 的控制速率。音频处理的 #GstElement 对象将使用此速率来细分其处理循环，并在其中调用 gst_object_sync_values()。处理段的长度应该达到 @control-rate 纳秒。
 *
 * 如果元素处于 %GST_STATE_PAUSED 或 %GST_STATE_PLAYING，则不应更改控制速率。
 */
void
gst_object_set_control_rate (GstObject * object, GstClockTime control_rate)
{
  g_return_if_fail (GST_IS_OBJECT (object));

  object->control_rate = control_rate;
}
