project('my_project', 'c', version: '1.0')

incs = include_directories('include')

cc = meson.get_compiler('c')

compile_args = ['-DCRT_SECURE_NO_DEPRECATE="sss"', '-DCRT_NONSTDC_NO_DEPRECATE']

# 也可以直接使用字符串赋值给link_args，使用编译器具备检测功能
link_args = cc.get_supported_link_arguments(
  '-Wl,--rpath,/usr/local:/usr',
  '-Wl,--rpath,/usr',
  '-Wl,--warn-common'
)

executable('demo', 'main.c',
           c_args: compile_args,
           link_args: link_args,
           include_directories: incs)
