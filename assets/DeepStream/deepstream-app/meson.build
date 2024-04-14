project('DeepStream APP', 'c', 'cpp',
        version: '3.0',
        default_options: [
          'buildtype=debugoptimized',
          'warning_level=1',
          # We only need c99, but glib needs GNU-specific features
          # https://github.com/mesonbuild/meson/issues/2289
          'c_std=gnu99',
        ],
        meson_version : '>= 0.60.0')


# 依赖 gstreamer-1.0 gstreamer-video-1.0 x11 json-glib-1.0
gstreamer_dep        = dependency('gstreamer-1.0')
gstreamer_video_dep  = dependency('gstreamer-video-1.0')
x11_dep              = dependency('x11')
json_glib_dep        = dependency('json-glib-1.0')

# 指定头文件目录
apps_inc = include_directories('.')
apps_common_inc = include_directories('../../apps-common/includes')
nvd_inc = include_directories('../../../includes')
cuda_inc = include_directories('/usr/local/cuda/include')

# 源文件
c_apps_sources = files(
  'deepstream_app.c',
  'deepstream_app_config_parser.c',
  'deepstream_app_main.c'
)

cpp_apps_sources = files(
  'deepstream_app_config_parser_yaml.cpp'
)

c_apps_common_sources = files(
  '../../apps-common/src/deepstream_dsanalytics.c',
  '../../apps-common/src/deepstream_tiled_display_bin.c',
  '../../apps-common/src/deepstream_primary_gie_bin.c',
  '../../apps-common/src/deepstream_dewarper_bin.c',
  '../../apps-common/src/deepstream_segvisual_bin.c',
  '../../apps-common/src/deepstream_audio_classifier_bin.c',
  '../../apps-common/src/deepstream_tracker_bin.c',
  '../../apps-common/src/deepstream_perf.c',
  '../../apps-common/src/deepstream_secondary_preprocess.c',
  '../../apps-common/src/deepstream_c2d_msg.c',
  '../../apps-common/src/deepstream_dsexample.c',
  '../../apps-common/src/deepstream_common.c',
  '../../apps-common/src/deepstream_secondary_gie_bin.c',
  '../../apps-common/src/deepstream_c2d_msg_util.c',
  '../../apps-common/src/deepstream_config_file_parser.c',
  '../../apps-common/src/deepstream_source_bin.c',
  '../../apps-common/src/deepstream_sink_bin.c',
  '../../apps-common/src/deepstream_streammux.c',
  '../../apps-common/src/deepstream_preprocess.c',
  '../../apps-common/src/deepstream_osd_bin.c',
)


cpp_apps_common_sources = files(
  '../../apps-common/src/deepstream-yaml/deepstream_image_save_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_msgconv_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_tiled_display_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_gie_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_streammux_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_osd_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_dsanalytics_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_source_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_dsexample_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_dewarper_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_msgconsumer_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_config_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_sink_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_tracker_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_preprocess_yaml.cpp',
  '../../apps-common/src/deepstream-yaml/deepstream_segvisual_yaml.cpp',
)


cc = meson.get_compiler('c')

bm_dep           = cc.find_library('m')
yaml_dep         = cc.find_library('yaml-cpp')
gstrtspserver_dp = cc.find_library('gstrtspserver-1.0')
bdl_dep          = cc.find_library('dl')

cuda_dep = cc.find_library('cudart', dirs: '/usr/local/cuda/lib64')
nvdsgst_meta_dep = cc.find_library('nvdsgst_meta', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvds_meta_dep = cc.find_library('nvds_meta', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvdsgst_helper_dep = cc.find_library('nvdsgst_helper', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvdsgst_customhelper_dep = cc.find_library('nvdsgst_customhelper', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvdsgst_smartrecord_dep = cc.find_library('nvdsgst_smartrecord', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvds_utils_dep = cc.find_library('nvds_utils', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')
nvds_msgbroker_dep = cc.find_library('nvds_msgbroker', dirs: '/opt/nvidia/deepstream/deepstream-6.4/lib')



# nvd_dep = declare_dependency(include_directories: '../../includes',
#                              link_args: ['-L/opt/nvidia/deepstream/deepstream-6.4/lib'],
#                              link_with: ['nvdsgst_meta', 'nvds_meta', 'nvdsgst_helper', 'nvdsgst_customhelper', 
#                                          'nvdsgst_smartrecord', 'nvds_utils', 'nvds_msgbroker'])

# cuda_dep = declare_dependency(include_directories: '/usr/local/cuda/include',
#                              link_args: ['-L/usr/local/cuda/lib64'],
#                              link_with: ['cudart'])

executable('vpf-deepstream-app',
  sources: [c_apps_sources, cpp_apps_sources, c_apps_common_sources, cpp_apps_common_sources],
  include_directories: [apps_inc, apps_common_inc, nvd_inc, cuda_inc],
  # c_args: test_args + common_cflags,
  dependencies: [gstreamer_dep, gstreamer_video_dep, x11_dep, json_glib_dep, 
                 cuda_dep, nvdsgst_meta_dep, nvds_meta_dep, nvdsgst_helper_dep, nvdsgst_customhelper_dep, nvdsgst_smartrecord_dep,
                 nvds_utils_dep, nvds_msgbroker_dep, bm_dep, yaml_dep, gstrtspserver_dp, bdl_dep],
)