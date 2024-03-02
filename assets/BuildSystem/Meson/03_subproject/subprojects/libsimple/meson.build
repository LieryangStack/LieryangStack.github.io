project('libsimple', 'cpp', version: '1.0')

inc = include_directories('include')
# 生成动态库 libsimple.so
libsimple = shared_library('simple',
                           'simple.cpp',
                           include_directories : inc,
                           install : true)

# declare_dependency返回一个dependency(.)对象
libsimple_dep = declare_dependency(include_directories : inc,
                                   link_with : libsimple)