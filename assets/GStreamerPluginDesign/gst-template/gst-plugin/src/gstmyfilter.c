/**
 * SECTION:element-myfilter
 *
 * FIXME:Describe myfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v -m fakesrc ! my_filter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#include "config.h"

#include <gst/gst.h>

#include "gstmyfilter.h"

#include <stdio.h>

GST_DEBUG_CATEGORY_STATIC (gst_my_filter_debug);
#define GST_CAT_DEFAULT gst_my_filter_debug


/* 元素信号 */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

/* 元素属性 */
enum
{
  PROP_0,
  PROP_SILENT
};


/**
 * GstStaticPadTemplate是通过G_DEFINE_POINTER_TYPE进行注册的
 * 
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory =  GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

// GST_STATIC_PAD_TEMPLATE ("src",
//     GST_PAD_SRC,
//     GST_PAD_ALWAYS,
//     GST_STATIC_CAPS
//     ( "audio/x-adpcm, layout = (string) swf, channels = (int) { 1, 2 }, rate = (int) { 5512, 11025, 22050, 44100 }; "
//       "audio/mpeg, mpegversion = (int) 1, layer = (int) 3, channels = (int) { 1, 2 }, rate = (int) { 5512, 8000, 11025, 22050, 44100 }, parsed = (boolean) TRUE; "
//       "audio/mpeg, mpegversion = (int) { 4, 2 }, stream-format = (string) raw; "
//       "audio/x-nellymoser, channels = (int) { 1, 2 }, rate = (int) { 5512, 8000, 11025, 16000, 22050, 44100 }; "
//       "audio/x-raw, format = (string) { U8, S16LE}, layout = (string) interleaved, channels = (int) { 1, 2 }, rate = (int) { 5512, 11025, 22050, 44100 }; "
//       "audio/x-alaw, channels = (int) { 1, 2 }, rate = (int) 8000; "
//       "audio/x-mulaw, channels = (int) { 1, 2 }, rate = (int) 8000; "
//       "audio/x-speex, channels = (int) 1, rate = (int) 16000;")
//     );


#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

/**
 * @param e: 元素的名称（小写）, 需要跟注册元素的时候 GST_ELEMENT_REGISTER 的第一个参数相同
 * @param e_n: 元素的名称，用于元素创建 gst_element_factory_make 函数中使用
 * @param r: 元素的等级
 * @param t: Gtype类型系统中已经被注册的类型
 */
GST_ELEMENT_REGISTER_DEFINE (my_filter, "my_filter", GST_RANK_NONE,
    GST_TYPE_MYFILTER);

static void gst_my_filter_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_my_filter_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_my_filter_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_filter_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);


static void
gst_my_filter_class_init (GstMyFilterClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_my_filter_set_property;
  gobject_class->get_property = gst_my_filter_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          TRUE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "MyFilter",
      "FIXME:Generic",
      "FIXME:Generic Template Element", "lieryang <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}


static void
gst_my_filter_init (GstMyFilter * filter) {

  /* 从 GstStaticPadTemplate 创建 sink GstPad */
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

  /* 给GstPad设定事件处理函数 */
  gst_pad_set_event_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_my_filter_sink_event));
  
  /* 给GstPad设定链函数 */
  gst_pad_set_chain_function (filter->sinkpad,
      GST_DEBUG_FUNCPTR (gst_my_filter_chain));
  
  /* 所有caps或者pad接受到的事件能够传给链接的节点，而不是丢弃 */
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  /*  从 GstStaticPadTemplate 创建 src GstPad */
  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = TRUE;
}

static void
gst_my_filter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_filter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstMyFilter *filter = GST_MYFILTER (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static gboolean
gst_my_filter_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstMyFilter *filter;
  gboolean ret;

  filter = GST_MYFILTER (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  printf ("@event@: %s Received %s event: \n", \
          gst_object_get_name (parent), GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}


static GstFlowReturn
gst_my_filter_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstMyFilter *filter;

  filter = GST_MYFILTER (parent);

  if (filter->silent == FALSE)
    g_print ("I'm plugged, therefore I'm in.\n");

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}


/**
 * @brief: 注册插件的时候，会调用该初始化函数，用于注册element
 */
static gboolean
myfilter_init (GstPlugin * plugin)
{
  
  GST_DEBUG_CATEGORY_INIT (gst_my_filter_debug, "myfilter",
      0, "Debug相关的详细描述要写在这里");

  /**
   * @param element: 元素的名称，需要跟 GST_ELEMENT_REGISTER_DEFINE 的第一个参数相同
   * @param plugin: 插件对象的地址
   */
  return GST_ELEMENT_REGISTER (my_filter, plugin);
}

/* 属于那类型的插件，比如： gst-plugins-base， gst-plugins-good */
#ifndef PACKAGE
#define PACKAGE "myfirstmyfilter"
#endif

/**
 * @brief: 在GStreamer系统中注册一个插件
 * @param major: 插件的主版本号
 * @param minor: 插件的次版本号
 * @param namen: 插件的名称，gst-inspect-1.0 可以使用@name 搜索到该插件
 * @param description: 该插件的相关描述
 * @param init: 插件的初始化函数
 * @param version: 插件的版本号
 * @param license: 插件的许可证
 * @param package: 插件包所在的包名
 * @param orign: 插件的来源
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myfilter,
    "学习如何编写插件",
    myfilter_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
