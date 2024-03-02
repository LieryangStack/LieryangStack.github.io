/*************************************************************************************************************************************************
 * @filename: gstmeta_foo.c
 * @author: EryangLi
 * @version: 1.0
 * @date: Nov-27-2023
 * @brief: 
 * 
 * @note: 
 * @history: 
 *          1. Date:
 *             Author:
 *             Modification:
 *      
 *          2. ..
 *************************************************************************************************************************************************
*/
#include <gst/gst.h>


typedef struct
{
  GstMeta meta;
} GstMetaFoo;

static GType gst_meta_foo_api_get_type (void);
#define GST_META_FOO_API_TYPE (gst_meta_foo_api_get_type())

static const GstMetaInfo *gst_meta_foo_get_info (void);
#define GST_META_FOO_INFO (gst_meta_foo_get_info())

#define GST_META_FOO_GET(buf) ((GstMetaFoo *)gst_buffer_get_meta(buf,GST_META_FOO_API_TYPE))
#define GST_META_FOO_ADD(buf) ((GstMetaFoo *)gst_buffer_add_meta(buf,GST_META_FOO_INFO,NULL))


static gboolean
foo_init_func (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GST_DEBUG ("init called on buffer %p, foo meta %p", buffer, meta);
  return TRUE;
}

static void
foo_free_func (GstMeta * meta, GstBuffer * buffer)
{
  GST_DEBUG ("free called on buffer %p, foo meta %p", buffer, meta);
}


static gboolean
foo_transform_func (GstBuffer * transbuf, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GST_DEBUG ("transform %s called from buffer %p to %p, meta %p",
      g_quark_to_string (type), buffer, transbuf, meta);

  if (GST_META_TRANSFORM_IS_COPY (type)) {
    GST_META_FOO_ADD (transbuf);
  } else {
    /* return FALSE, if transform type is not supported */
    return FALSE;
  }
  return TRUE;
}

static GType
gst_meta_foo_api_get_type (void)
{
  static GType type;
  static const gchar *tags[] = { NULL };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstMetaFooAPI", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

static const GstMetaInfo *
gst_meta_foo_get_info (void)
{
  static const GstMetaInfo *meta_foo_info = NULL;

  if (g_once_init_enter ((GstMetaInfo **) & meta_foo_info)) {
    const GstMetaInfo *mi = gst_meta_register (GST_META_FOO_API_TYPE,
        "GstMetaFoo",
        sizeof (GstMetaFoo),
        foo_init_func, foo_free_func, foo_transform_func);
    g_once_init_leave ((GstMetaInfo **) & meta_foo_info, (GstMetaInfo *) mi);
  }

  /* meta_foo_info->api是GST_META_FOO_API_TYPE，是“GstMetaFooAPI”*/
  g_print ("meta_foo_info->api = %ld\n", meta_foo_info->api);
  g_print ("GST_META_FOO_API_TYPE = %ld\n", GST_META_FOO_API_TYPE);
  /* meta_foo_info->type是“GstMetaFoo” */
  g_print ("meta_foo_info->type = %ld\n", meta_foo_info->type);

  return meta_foo_info;
}


int 
main (int argc, char *argv[]) {

  GstBuffer *buffer, *copy, *subbuf;
  GstMetaFoo *meta;
  GstMapInfo info;

  gst_init(&argc, &argv);

  buffer = gst_buffer_new_and_alloc (4);

  gst_buffer_map (buffer, &info, GST_MAP_WRITE);

  memset (info.data, 0, 4);
  gst_buffer_unmap (buffer, &info);

  /* add some metadata */
  meta = GST_META_FOO_ADD (buffer);



  g_print ("GST_MINI_OBJECT(buffer)->refcount = %d\n", GST_MINI_OBJECT(buffer)->refcount);
  gst_buffer_unref (buffer);

  return 0;
}