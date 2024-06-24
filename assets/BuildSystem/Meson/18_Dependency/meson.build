project('my_project', 'cpp', version: '1.0')

qt5_dep = dependency('qt5', modules : ['Core', 'Gui'], method : 'config-tool')
gstreamer_dep        = dependency('gstreamer-1.0', method: 'pkg-config')
gstreamer_video_dep  = dependency('gstreamer-video-1.0', version: '>=1.20')

# 返回该依赖是否被发现
if gstreamer_dep.found()
  message ('发现GStreamer')
else
  message ('没有找到GStreamer')
endif

if qt5_dep.found()
  message ('发现qt5_dep')
else
  message ('没有找到qt5_dep')
endif


gstreamer_dep_prefix = gstreamer_dep.get_pkgconfig_variable('libdir',  default: 'not fount @var_name', define_variable: ['prefix', '/tmp'])

message ('GStreamer前缀：', gstreamer_dep_prefix)

executable('demo', 'main.cpp',
           dependencies: [gstreamer_dep, gstreamer_video_dep])
