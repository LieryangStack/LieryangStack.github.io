#include<glib.h>

gboolean
source_prepare_cb(GSource *source, gint *timeout){
  g_print("prepare\n");
  *timeout = 1000;
  return FALSE;
}

gboolean
source_check_cb(GSource *source){
  g_print("check\n");
  return TRUE;
}

gboolean
source_dispatch_cb(GSource *source,
                   GSourceFunc callback,
                   gpointer data){
  g_print("dispatch\n");
  callback(data);
  return TRUE;                  
}

void
source_finalize_cb(GSource *source){
  g_print("finalize\n");
}

void
myidle(gpointer data){

  g_print("myidle\n");
}

int
main(int argc, char *argv[]){
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GMainContext *context = g_main_loop_get_context(loop);

    GSourceFuncs g_source_myidle_funcs = {
        source_prepare_cb,
        source_check_cb,
        source_dispatch_cb,
        source_finalize_cb,
    };

    /* 创建新事件源实例，传入了事件的函数表、事件结构体大小 */
    GSource *source = g_source_new(&g_source_myidle_funcs, sizeof(GSource));
    /* 设置新事件源source的回调函数 */
    g_source_set_callback(source, (GSourceFunc)myidle, "Hello, world!", NULL);
    /* source关联特定的GMainContext对象 */
    g_source_attach(source, context);
    g_source_unref(source);

    g_main_loop_run(loop);

    g_main_context_unref(context);
    g_main_loop_unref(loop);

    return 0;
}

