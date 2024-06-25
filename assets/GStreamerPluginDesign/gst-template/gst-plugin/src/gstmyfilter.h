#ifndef __GST_MYFILTER_H__
#define __GST_MYFILTER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_MYFILTER (gst_my_filter_get_type())
G_DECLARE_FINAL_TYPE (GstMyFilter, gst_my_filter,
    GST, MYFILTER, GstElement)

struct _GstMyFilter
{
  GstElement element;

  GstPad *sinkpad, *srcpad;

  gboolean silent;
};

G_END_DECLS

#endif /* __GST_MYFILTER_H__ */
