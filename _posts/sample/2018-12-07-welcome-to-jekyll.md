---
layout: post
title: Welcome to Jekyll!
subtitle: A awesome static site generator.
author: Jeffrey
categories: sample
banner:
  video: https://vjs.zencdn.net/v/oceans.mp4
  loop: true
  volume: 0.8
  start_at: 8.5
  image: https://bit.ly/3xTmdUP
  opacity: 0.618
  background: "#000"
  height: "100vh"
  min_height: "38vh"
  heading_style: "font-size: 4.25em; font-weight: bold; text-decoration: underline"
  subheading_style: "color: gold"
tags: sample
sidebar: []
---

You’ll find this post in your `_posts` directory. Go ahead and edit it and re-build the site to see your changes. You can rebuild the site in many different ways, but the most common way is to run `jekyll serve`, which launches a web server and auto-regenerates your site when a file is updated.

To add new posts, simply add a file in the `_posts` directory that follows the convention `YYYY-MM-DD-name-of-post.ext` and includes the necessary front matter. Take a look at the source for this post to get an idea about how it works.

## section 1

Jekyll also offers powerful support for code snippets:

{% highlight ruby %}
def print_hi(name)
puts "Hi, #{name}"
end
print_hi('Tom')
#=> prints 'Hi, Tom' to STDOUT.
{% endhighlight %}

## section 2

Check out the [Jekyll docs][jekyll-docs] for more info on how to get the most out of Jekyll. File all bugs/feature requests at [Jekyll’s GitHub repo][jekyll-gh]. If you have questions, you can ask them on [Jekyll Talk][jekyll-talk].

[jekyll-docs]: https://jekyllrb.com/docs/home
[jekyll-gh]: https://github.com/jekyll/jekyll
[jekyll-talk]: https://talk.jekyllrb.com/

$ a \* b = c ^ b $

$ 2^{\frac{n-1}{3}} $

$ \int_a^b f(x)\,dx. $

```cpp
#include <iostream>
using namespace std;

int main() {
  cout << "Hello World!";
  return 0;
}
// prints 'Hi, Tom' to STDOUT.
```

```c
/**

 * Copyright (C), 2020-2099, Lieryng Tech. Co., Ltd.

 * FileName: 04_Memory_allocation.c

 * Author:  Lieryang   Version :  1.0    Date:Oct-26-2023

 * Description:      1.测试GstMemory对象的相关特性
                     2.测试GstBuffer对象的相关特性
                     3.测试GstMeta对象的相关特性
                     4.测试GstBufferPool对象的相关特性
 * Version:         // 版本信息

 * Function List:   // 主要函数及其功能

    1. -------

 * History:         // 历史修改记录

      <author>  <time>   <version >   <desc>


 */

#include <gst/gst.h>
#include <gst/video/gstvideometa.h>

gboolean
print_config_structure(GQuark   field_id,
                       const GValue * value,
                       gpointer user_data){

  g_print("****foreach****\n");
  g_print("%s\n", G_VALUE_TYPE_NAME(value));
  g_print("%s\n",g_quark_to_string (field_id));

  if( g_type_is_a(value->g_type, G_TYPE_UINT))
    g_print("%s = %d\n",g_quark_to_string (field_id), g_value_get_uint(value));
  // else
  //   g_print("%s, %s\n",g_quark_to_string (field_id), g_value_get_string(value));

  return TRUE;
}

int
main (int argc, char *argv[]) {

  gst_init (&argc, &argv);

  GstMemory *mem1, *mem2;
  GstMapInfo info;
  gint i;

  /* 参数allocator = NULL将使用默认的分配器 */
  mem1 = gst_allocator_alloc (NULL, 100, NULL);
  mem2 = gst_allocator_alloc (NULL, 50, NULL);

  g_print ("Memory type: %s\n", mem1->allocator->mem_type);

  g_print ("mem->maxsize = %ld\n", mem1->maxsize);
  g_print ("mem->size = %ld\n", mem1->size);
  g_print ("mem->offset = %ld\n", mem1->offset);
  /* 暂时没有了解对齐的原因 */
  g_print ("mem->align = %ld\n", mem1->align);
  g_print ("mem->parent = %p\n", mem1->parent);

  gst_memory_map (mem1, &info, GST_MAP_WRITE);

  for (i = 0; i < info.size; i++)
    info.data[i] = i;

  /* release memory */
  gst_memory_unmap (mem1, &info);

  /************测试GstBuffer***************/
  GstBuffer *buffer = gst_buffer_new ();
  gst_buffer_append_memory (buffer, mem1);
  gst_buffer_append_memory (buffer, mem2);

  /* 这里会合并所有GstMemory */
  gst_buffer_map (buffer, &info, GST_MAP_WRITE);
  g_print ("info.maxsize = %ld\n", info.maxsize);
  g_print ("info.size = %ld\n", info.size);

  // memset (info.data, 0xff, info.size);
  gst_buffer_unmap (buffer, &info);

  /**************GstMeta****************/
  
  /* buffer points to a video frame, add some cropping metadata */
  GstVideoCropMeta *meta = gst_buffer_add_video_crop_meta (buffer);
  /* configure the cropping metadata */
  meta->x = 8;
  meta->y = 8;
  meta->width = 120;
  meta->height = 80;
  
  gst_buffer_unref (buffer);

  gst_allocator_free (mem1->allocator, mem1);
  gst_allocator_free (mem2->allocator, mem2);

  g_print ("mem1->mini_object.refcount = %d\n", mem1->mini_object.refcount);
  g_print ("mem2->mini_object.refcount = %d\n", mem2->mini_object.refcount);

  /*************GstBufferPool**************/
  GstBufferPool *pool = gst_buffer_pool_new();;

  GstStructure *config = gst_buffer_pool_get_config (pool);

  GstAllocator *allocator = gst_allocator_find (NULL);

  GstAllocationParams params = {0, 0, 0, 0};

  /* set caps, size, minimum and maximum buffers in the pool */
  /***
   * 如果最大最小设置为0，则可以无限请求buffer
   * 如果目前使用中的buffer达到了最大数量，该线程就会阻塞等待buffer释放。
  */
  gst_buffer_pool_config_set_params (config, NULL, 60, 0, 56);

  /* configure allocator and parameters */
  gst_buffer_pool_config_set_allocator (config, allocator, &params);

  /* 查看config结构体里面的所有键值对 */
  gst_structure_foreach(config, print_config_structure, NULL);

  /* store the updated configuration again */
  gst_buffer_pool_set_config (pool, config);

  gst_buffer_pool_set_active (pool, TRUE);


  for (guint i = 1; i < 100; i++) {
    g_print ("i = %d ", i);
    guint ret = gst_buffer_pool_acquire_buffer (pool, &buffer, NULL);
    g_print ("ret = %d\n", ret);
    if (G_UNLIKELY (ret != GST_FLOW_OK))
      g_print (" acquire buffer failed\n");
  }

  gst_buffer_map (buffer, &info, GST_MAP_WRITE);
  g_print ("info.maxsize = %ld\n", info.maxsize);
  g_print ("info.size = %ld\n", info.size);
  

  // memset (info.data, 0xff, info.size);
  gst_buffer_unmap (buffer, &info);

  /* 查看config结构体里面的所有键值对 */
  gst_structure_foreach(config, print_config_structure, NULL);

  gst_buffer_pool_release_buffer(pool, buffer);

  gst_object_unref (pool);
  
  return 0;
}
```
