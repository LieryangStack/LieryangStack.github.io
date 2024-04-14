/*
 * Copyright (c) 2018-2023, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVGSTDS_APP_H__
#define __NVGSTDS_APP_H__

#include <gst/gst.h>
#include <stdio.h>

#include "deepstream_app_version.h"
#include "deepstream_common.h"
#include "deepstream_config.h"
#include "deepstream_osd.h"
#include "deepstream_segvisual.h"
#include "deepstream_perf.h"
#include "deepstream_preprocess.h"
#include "deepstream_primary_gie.h"
#include "deepstream_sinks.h"
#include "deepstream_sources.h"
#include "deepstream_streammux.h"
#include "deepstream_tiled_display.h"
#include "deepstream_dsanalytics.h"
#include "deepstream_dsexample.h"
#include "deepstream_tracker.h"
#include "deepstream_secondary_gie.h"
#include "deepstream_secondary_preprocess.h"
#include "deepstream_c2d_msg.h"
#include "deepstream_image_save.h"
#include "gst-nvdscustommessage.h"
#include "gst-nvdscommonconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _AppCtx AppCtx;

typedef void (*bbox_generated_callback) (AppCtx *appCtx, GstBuffer *buf,
    NvDsBatchMeta *batch_meta, guint index);
typedef gboolean (*overlay_graphics_callback) (AppCtx *appCtx, GstBuffer *buf,
    NvDsBatchMeta *batch_meta, guint index);

typedef struct
{
  guint index;
  gulong all_bbox_buffer_probe_id;
  gulong primary_bbox_buffer_probe_id;
  gulong fps_buffer_probe_id;
  GstElement *bin;
  GstElement *tee;
  GstElement *msg_conv;
  NvDsPreProcessBin preprocess_bin;
  NvDsPrimaryGieBin primary_gie_bin;
  NvDsOSDBin osd_bin;
  NvDsSegVisualBin segvisual_bin;
  NvDsSecondaryGieBin secondary_gie_bin;
  NvDsSecondaryPreProcessBin secondary_preprocess_bin;
  NvDsTrackerBin tracker_bin;
  NvDsSinkBin sink_bin;
  NvDsSinkBin demux_sink_bin;
  NvDsDsAnalyticsBin dsanalytics_bin;
  NvDsDsExampleBin dsexample_bin;
  AppCtx *appCtx;
} NvDsInstanceBin;

typedef struct
{
  gulong primary_bbox_buffer_probe_id;
  guint bus_id;
  GstElement *pipeline;
  NvDsSrcParentBin multi_src_bin;
  NvDsInstanceBin instance_bins[MAX_SOURCE_BINS];
  NvDsInstanceBin demux_instance_bins[MAX_SOURCE_BINS];
  NvDsInstanceBin common_elements;
  GstElement *tiler_tee;
  NvDsTiledDisplayBin tiled_display_bin;
  GstElement *demuxer;
  NvDsDsExampleBin dsexample_bin;
  AppCtx *appCtx;
} NvDsPipeline;


/**
 * @NvDsConfig: NVIDIA DeepStream的配置结构体，cfg_files文件的配置信息会解析到该数组中，用于存储多种视频流处理组件的配置信息。
 * @enable_perf_measurement: 是否在控制台打印输出性能测量数据（帧数）。
 * @file_loop: 文件循环次数，控制输入文件的循环播放次数。
 * @pipeline_recreate_sec: 定义在多少秒后重建管道。
 * @source_list_enabled: 是否启用源列表
 * @total_num_sources: 总视频源数量
 * 
 * 数组使用了的有效长度（有些配置组是多个 source secondary-pre-process secondary-gie message-consumer sink）
 * @num_source_sub_bins: 视频源配置组数量
 * @num_secondary_gie_sub_bins: 二级推理模型的数量
 * @num_secondary_preprocess_sub_bins: 二级推理模型前预处理配置组的数量
 * @num_sink_sub_bins: sink输出配置组的数量
 * @num_message_consumers: 消息输出配置组信息的数量
 * 
 * @perf_measurement_interval_sec: 性能测量的时间间隔（秒）
 * @sgie_batch_size: 二级推理模型批处理大小。
 * @bbox_dir_path: 边界框目录路径，用于存储边界框数据
 * @kitti_track_dir_path: KITTI追踪数据的目录路径。
 * @reid_track_dir_path: Re-identification追踪数据的目录路径
 * @terminated_track_output_path: 终止追踪的输出路径
 * @shadow_track_output_path: 阴影追踪的输出路径
 * @uri_list: URI列表，指向视频源的URI数组
 * @sensor_id_list: 传感器ID列表
 * @sensor_name_list: 传感器名称列表
 * @multi_source_config: 视频源配置组信息 [source%d]
 * @streammux_config: 流多路复用器配置 [streammux]
 * @osd_config: OSD（On-Screen Display）配置组信息 [osd]
 * @segvisual_config: 语言分割可视化配置组信息 [segvisual]
 * @preprocess_config: 一级推理模型前预处理配置组信息  [pre-process]
 * @secondary_preprocess_sub_bin_config: 二级推理默前预处理配置组信息 [secondary-pre-process%d]
 * @primary_gie_config: 一级模推理型配置组信息 [primary-gie]
 * @tracker_config: 追踪器配置组信息 [tracker]
 * @secondary_gie_sub_bin_config: 二级推理模型配置组信息 [secondary-gie%d]
 * @sink_bin_sub_bin_config: sink输出配置组信息 [sink%d]
 * @message_consumer_config: 消息输出配置组信息 [message-consumer%d]
 * @tiled_display_config: 平铺显示配置组信息 [tiled-display]
 * @dsanalytics_config: 分析配置组信息 [nvds-analytics]
 * @dsexample_config: gst-dsexample插件配置组信息 [ds-example]
 * @msg_conv_config:  配置组信息[message-converter]
 * @image_save_config: 配置组信息[img-save]
 * @use_nvmultiurisrcbin: 是否使用nvmultiurisrcbin组件
 * @stream_name_display: 是否显示流名称flag
 * @max_batch_size: 最大批处理大小。
 * @http_ip: HTTP服务器IP地址
 * @http_port: HTTP服务器端口。
 * @source_attr_all_parsed: 所有源属性是否已解析
 * @source_attr_all_config: 所有源的属性配置
 * @global_gpu_id: 全局GPU ID，用于所有组件（如果未为单个组件设置gpu_id）。
 */
typedef struct
{
  gboolean enable_perf_measurement;
  gint file_loop;
  gint pipeline_recreate_sec;
  gboolean source_list_enabled;
  guint total_num_sources;
  guint num_source_sub_bins;
  guint num_secondary_gie_sub_bins;
  guint num_secondary_preprocess_sub_bins;
  guint num_sink_sub_bins;
  guint num_message_consumers;
  guint perf_measurement_interval_sec;
  guint sgie_batch_size;
  gchar *bbox_dir_path;
  gchar *kitti_track_dir_path;
  gchar *reid_track_dir_path;
  gchar *terminated_track_output_path;
  gchar *shadow_track_output_path;

  gchar **uri_list;
  gchar **sensor_id_list;
  gchar **sensor_name_list;
  NvDsSourceConfig multi_source_config[MAX_SOURCE_BINS];
  NvDsStreammuxConfig streammux_config;
  NvDsOSDConfig osd_config;
  NvDsSegVisualConfig segvisual_config;
  NvDsPreProcessConfig preprocess_config;
  NvDsPreProcessConfig secondary_preprocess_sub_bin_config[MAX_SECONDARY_PREPROCESS_BINS];
  NvDsGieConfig primary_gie_config;
  NvDsTrackerConfig tracker_config;
  NvDsGieConfig secondary_gie_sub_bin_config[MAX_SECONDARY_GIE_BINS];
  NvDsSinkSubBinConfig sink_bin_sub_bin_config[MAX_SINK_BINS];
  NvDsMsgConsumerConfig message_consumer_config[MAX_MESSAGE_CONSUMERS];
  NvDsTiledDisplayConfig tiled_display_config;
  NvDsDsAnalyticsConfig dsanalytics_config;
  NvDsDsExampleConfig dsexample_config;
  NvDsSinkMsgConvBrokerConfig msg_conv_config;
  NvDsImageSave image_save_config;

  /** To support nvmultiurisrcbin */
  gboolean use_nvmultiurisrcbin;
  gboolean stream_name_display;
  guint max_batch_size;
  gchar* http_ip;
  gchar* http_port;
  gboolean source_attr_all_parsed;
  NvDsSourceConfig source_attr_all_config;

  /** To set Global GPU ID for all the componenents at once if needed
   * This will be used in case gpu_id prop is not set for a component
   * if gpu_id prop is set for a component, global_gpu_id will be overridden by it */
  gint global_gpu_id;
} NvDsConfig;

typedef struct
{
  gulong frame_num;
} NvDsInstanceData;

/**
 * @_AppCtx: 应用程序上下文，包含控制和配置信息
 * @param version: 版本控制的布尔标志
 * @param cintr: 中断控制的布尔标志
 * @param show_bbox_text: 是否显示边界框文本的布尔标志
 * @param seeking: 搜索状态的布尔标志
 * @param quit: 退出程序的布尔标志
 * @param person_class_id: 人物分类标识符
 * @param car_class_id: 汽车分类标识符
 * @param return_value: 程序返回值
 * @param index: 实例数组的索引，用于标识主程序定义的AppCtx[MAX_INSTANCES]数组中的位置
 * @param active_source_index: 窗口显示视频源索引，-1表示平铺所有视频画面，其余0和正整数表示显示相应的摄像头画面
 * 
 * @param app_lock: 应用程序的互斥锁
 * @param app_cond: 应用程序的条件变量
 * 
 * @param pipeline: NvDsPipeline，整个管道Pipeline Bin Element指针都存储在该结构体中
 * @param config: cfg_files文件的配置信息会解析到该数组中
 * @param override_config: 可能通过OTA或者其他自定义函数修改配置组信息，其实就是@config的可修改副本，存储修改后的配置组信息
 * @param instance_data: 每个源的实例数据数组
 * @param c2d_ctx: 消费者上下文数组，用于处理来自不同消息消费者的数据
 * @param perf_struct: 性能结构体，用于监测性能相关数据
 * 
 * @param bbox_generated_post_analytics_cb: 边界框生成后的回调函数
 * @param all_bbox_generated_cb: 所有边界框生成的回调函数
 * @param overlay_graphics_cb: 覆盖图形的回调函数
 * 
 * @param latency_info: 帧延迟信息
 * @param latency_lock: 帧延迟信息的锁
 * @param ota_handler_thread: OTA处理线程
 * @param ota_inotify_fd: OTA inotify文件描述符
 * @param ota_watch_desc: OTA监视描述符
 * 
 * @param sensorInfoHash: 哈希表，保存通过REST API流添加/移除操作获得的NvDsSensorInfo，键是source_id
 */
struct _AppCtx
{
  gboolean version;
  gboolean cintr;
  gboolean show_bbox_text;
  gboolean seeking;
  gboolean quit;
  gint person_class_id;
  gint car_class_id;
  gint return_value;
  guint index; 
  gint active_source_index; 

  GMutex app_lock;
  GCond app_cond;

  NvDsPipeline pipeline;
  NvDsConfig config;
  NvDsConfig override_config;
  NvDsInstanceData instance_data[MAX_SOURCE_BINS];
  NvDsC2DContext *c2d_ctx[MAX_MESSAGE_CONSUMERS];
  NvDsAppPerfStructInt perf_struct;
  
  bbox_generated_callback bbox_generated_post_analytics_cb;
  bbox_generated_callback all_bbox_generated_cb;
  overlay_graphics_callback overlay_graphics_cb;
  
  NvDsFrameLatencyInfo *latency_info;
  GMutex latency_lock;
  GThread *ota_handler_thread;
  guint ota_inotify_fd;
  guint ota_watch_desc;

  /** Hash table to save NvDsSensorInfo
   * obtained with REST API stream/add, remove operations
   * The key is souce_id */
  GHashTable *sensorInfoHash;
};

/**
 * @brief  Create DS Anyalytics Pipeline per the appCtx
 *         configurations
 * @param  appCtx [IN/OUT] The application context
 *         providing the config info and where the
 *         pipeline resources are maintained
 * @param  bbox_generated_post_analytics_cb [IN] This callback
 *         shall be triggered after analytics
 *         (PGIE, Tracker or the last SGIE appearing
 *         in the pipeline)
 *         More info: create_common_elements()
 * @param  all_bbox_generated_cb [IN]
 * @param  perf_cb [IN]
 * @param  overlay_graphics_cb [IN]
 */
gboolean create_pipeline (AppCtx * appCtx,
    bbox_generated_callback bbox_generated_post_analytics_cb,
    bbox_generated_callback all_bbox_generated_cb,
    perf_callback perf_cb,
    overlay_graphics_callback overlay_graphics_cb);

gboolean pause_pipeline (AppCtx * appCtx);
gboolean resume_pipeline (AppCtx * appCtx);
gboolean seek_pipeline (AppCtx * appCtx, glong milliseconds, gboolean seek_is_relative);

void toggle_show_bbox_text (AppCtx * appCtx);

void destroy_pipeline (AppCtx * appCtx);
void restart_pipeline (AppCtx * appCtx);


/**
 * Function to read properties from configuration file.
 *
 * @param[in] config pointer to @ref NvDsConfig
 * @param[in] cfg_file_path path of configuration file.
 *
 * @return true if parsed successfully.
 */
gboolean
parse_config_file (NvDsConfig * config, gchar * cfg_file_path);

/**
 * Function to read properties from YML configuration file.
 *
 * @param[in] config pointer to @ref NvDsConfig
 * @param[in] cfg_file_path path of configuration file.
 *
 * @return true if parsed successfully.
 */
gboolean
parse_config_file_yaml (NvDsConfig * config, gchar * cfg_file_path);

/**
 * Function to procure the NvDsSensorInfo for the source_id
 * that was added using the nvmultiurisrcbin REST API
 *
 * @param[in] appCtx [IN/OUT] The application context
 *            providing the config info and where the
 *            pipeline resources are maintained
 * @param[in] source_id [IN] The unique source_id found in NvDsFrameMeta
 *
 * @return [transfer-floating] The NvDsSensorInfo for the source_id
 * that was added using the nvmultiurisrcbin REST API.
 * Please note that the returned pointer
 * will be valid only until the stream is removed.
 */
NvDsSensorInfo* get_sensor_info(AppCtx* appCtx, guint source_id);

#ifdef __cplusplus
}
#endif

#endif
