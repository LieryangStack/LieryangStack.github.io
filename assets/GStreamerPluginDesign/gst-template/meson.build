project('gst-template', 'c', 
  version : '1.22.6', 
  license : 'LGPL',
  default_options : ['buildtype=debug'])

plugins_install_dir = '/opt/gstreamer/lib/x86_64-linux-gnu/gstreamer-1.0'

cc = meson.get_compiler('c')

gst_version = meson.project_version()

api_version = '1.0'

gst_dep = dependency('gstreamer-1.0', version : '>=1.22',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.22',
  fallback : ['gstreamer', 'gst_base_dep'])

subdir('gst-plugin')
