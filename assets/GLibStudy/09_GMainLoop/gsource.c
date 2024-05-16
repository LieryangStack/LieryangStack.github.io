#include<glib.h>

/************************自定义源类型**************************/

gboolean
source_prepare_cb(GSource *source, gint *timeout){
  g_print("%s\n",__func__);
  *timeout = 100;
  return TRUE;
}

gboolean
source_check_cb(GSource *source){
  g_print("%s\n",__func__);
  return TRUE;
}

gboolean
source_dispatch_cb(GSource *source,
                   GSourceFunc callback,
                   gpointer data){
  g_print("%s\n",__func__);
  callback(data);
  return TRUE;                  
}

void
source_finalize_cb(GSource *source){
  g_print("%s\n",__func__);
}

void
source_cb(gpointer data){

  g_print("%s\n",__func__);
}

/************************自定义空闲源类型************************/

gboolean
idle_source_prepare_cb(GSource *source, gint *timeout){
  g_print("%s\n",__func__);
  *timeout = 200;
  return TRUE;
}

gboolean
idle_source_check_cb(GSource *source){
  g_print("%s\n",__func__);
  return TRUE;
}

gboolean
idle_source_dispatch_cb(GSource *source,
                   GSourceFunc callback,
                   gpointer data){
  g_print("%s\n",__func__);
  callback(data);
  return TRUE;                  
}

void
idle_source_finalize_cb(GSource *source){
  g_print("%s\n",__func__);
}

void
idle_source_cb(gpointer data){

  g_print("%s\n",__func__);
}



gboolean 
timeout_cb (gpointer user_data) {
  return TRUE;
}

int
main(int argc, char *argv[]){
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GMainContext *context = g_main_loop_get_context(loop);

    GSourceFuncs g_source_myidle_funcs = {
        idle_source_prepare_cb,
        idle_source_check_cb,
        idle_source_dispatch_cb,
        idle_source_finalize_cb,
    };

    GSourceFuncs g_source_funcs = {
        source_prepare_cb,
        source_check_cb,
        source_dispatch_cb,
        source_finalize_cb,
    };

    /* 创建新事件源实例，传入了事件的函数表、事件结构体大小 */
    GSource *source = g_source_new(&g_source_funcs, sizeof(GSource));
    g_print ("g_source_new After ref_count = %d\n", source->ref_count); /* ref_count = 1 */
    /* 设置新事件源source的回调函数 */
    g_source_set_callback(source, (GSourceFunc)source_cb, "Hello, world!", NULL);
    g_print ("g_source_set_callback After ref_count = %d\n", source->ref_count); /* ref_count = 1 */
    /* source关联特定的GMainContext对象 */
    g_source_attach(source, context);
    g_print ("g_source_attach After ref_count = %d\n", source->ref_count); /* ref_count = 2 */
    g_source_unref(source);
    g_print ("g_source_unref After ref_count = %d\n", source->ref_count); /* ref_count = 1 */

    /**
     * 最后解引用 context的时候，context会调用 g_source_unref
    */

    GSource *idle_source = g_source_new(&g_source_myidle_funcs, sizeof(GSource));
    /* 设置新事件源source的回调函数 */
    g_source_set_callback(idle_source, (GSourceFunc)idle_source_cb, "Hello, world!", NULL);
    /* source关联特定的GMainContext对象 */
    g_source_attach(idle_source, context);
    g_source_unref(idle_source);

    

    // g_timeout_add_seconds (1, (GSourceFunc)g_main_loop_quit, loop);

    

    g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

    g_main_loop_run(loop);

    g_main_context_unref(context);
    g_main_loop_unref(loop);

    

    return 0;
}

