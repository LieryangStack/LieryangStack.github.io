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
    /* 设置新事件源source的回调函数 */
    g_source_set_callback(source, (GSourceFunc)source_cb, "Hello, world!", NULL);
    /* source关联特定的GMainContext对象 */
    g_source_attach(source, context);
    g_source_unref(source);

    source = g_source_new(&g_source_myidle_funcs, sizeof(GSource));
    /* 设置新事件源source的回调函数 */
    g_source_set_callback(source, (GSourceFunc)idle_source_cb, "Hello, world!", NULL);
    /* source关联特定的GMainContext对象 */
    g_source_attach(source, context);
    g_source_unref(source);


    // g_timeout_add_seconds (1, (GSourceFunc)g_main_loop_quit, loop);

    // g_idle_add

    g_timeout_add (1, (GSourceFunc)g_main_loop_quit, loop);

    g_main_loop_run(loop);

    g_main_context_unref(context);
    g_main_loop_unref(loop);

    

    return 0;
}

