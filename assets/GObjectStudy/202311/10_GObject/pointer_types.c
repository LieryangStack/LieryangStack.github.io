#include <glib-object.h>

typedef struct _GstStaticPadTemplate GstStaticPadTemplate;

struct _GstStaticPadTemplate
{
  gint a;
};

G_DEFINE_POINTER_TYPE (GstStaticPadTemplate, gst_static_pad_template);

int
main () {

}

