/*
 * Copyright (c) 2018-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVGSTDS_GIE_H__
#define __NVGSTDS_GIE_H__

#include <gst/gst.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "gstnvdsmeta.h"
#include "gstnvdsinfer.h"
#include "deepstream_config.h"

typedef enum
{
  NV_DS_GIE_PLUGIN_INFER = 0,
  NV_DS_GIE_PLUGIN_INFER_SERVER,
} NvDsGiePluginType;


/**
 * @NvDsGieConfig: 用于配置GPU推理引擎（GIE）的结构体。
 * @enable: 是否启用GIE。
 * @config_file_path: GIE配置文件的路径。
 * @input_tensor_meta: 是否输入张量元数据。
 * @override_colors: 是否覆盖边框和背景颜色。
 * @operate_on_gie_id: 操作的GIE ID。
 * @is_operate_on_gie_id_set: 是否设置了操作的GIE ID。
 * @operate_on_classes: 操作的类别。
 * @num_operate_on_class_ids: 操作的类别ID数量。
 * @list_operate_on_class_ids: 操作的类别ID列表。
 * @have_bg_color: 是否有背景颜色。
 * @bbox_bg_color: 边框背景颜色参数。
 * @bbox_border_color: 边框颜色参数。
 * @bbox_border_color_table: 边框颜色的哈希表。
 * @bbox_bg_color_table: 背景颜色的哈希表。
 * @batch_size: 批处理大小。
 * @is_batch_size_set: 是否设置了批处理大小。
 * @interval: 推理间隔。
 * @is_interval_set: 是否设置了推理间隔。
 * @unique_id: 唯一ID。
 * @is_unique_id_set: 是否设置了唯一ID。
 * @gpu_id: 使用的GPU ID。
 * @is_gpu_id_set: 是否设置了GPU ID。
 * @nvbuf_memory_type: 缓冲区内存类型。
 * @model_engine_file_path: 模型引擎文件路径。
 * @audio_transform: 音频转换设置。
 * @frame_size: 音频帧大小。
 * @is_frame_size_set: 是否设置了音频帧大小。
 * @hop_size: 音频跳跃大小。
 * @is_hop_size_set: 是否设置了音频跳跃大小。
 * @input_audio_rate: 输入音频率。
 * @label_file_path: 标签文件路径。
 * @n_labels: 标签数量。
 * @n_label_outputs: 输出的标签数量。
 * @labels: 标签数组。
 * @raw_output_directory: 原始输出目录。
 * @file_write_frame_num: 写入文件的帧号。
 * @tag: 标签或标记。
 * @plugin_type: 插件类型，定义了使用的GIE插件类型。
 */
typedef struct
{
  gboolean enable;

  gchar *config_file_path;

  gboolean input_tensor_meta;

  gboolean override_colors;

  gint operate_on_gie_id;
  gboolean is_operate_on_gie_id_set;
  gint operate_on_classes;

  gint num_operate_on_class_ids;
  gint *list_operate_on_class_ids;

  gboolean have_bg_color;
  NvOSD_ColorParams bbox_bg_color;
  NvOSD_ColorParams bbox_border_color;

  GHashTable *bbox_border_color_table;
  GHashTable *bbox_bg_color_table;

  guint batch_size;
  gboolean is_batch_size_set;

  guint interval;
  gboolean is_interval_set;
  guint unique_id;
  gboolean is_unique_id_set;
  guint gpu_id;
  gboolean is_gpu_id_set;
  guint nvbuf_memory_type;
  gchar *model_engine_file_path;

  gchar *audio_transform;
  guint frame_size;
  gboolean is_frame_size_set;
  guint hop_size;
  gboolean is_hop_size_set;
  guint input_audio_rate;

  gchar *label_file_path;
  guint n_labels;
  guint *n_label_outputs;
  gchar ***labels;

  gchar *raw_output_directory;
  gulong file_write_frame_num;

  gchar *tag;

  NvDsGiePluginType plugin_type;
} NvDsGieConfig;

#ifdef __cplusplus
}
#endif

#endif
