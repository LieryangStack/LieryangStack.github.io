# 'tutorial' 整个项目的名称， `c`编程语言是c语言， version表示该项目的版本
project('tutorial', 'c',
  version : '1.0.0',
  meson_version : '>= 0.60.0',
  default_options : [
    'prefix=/home/lieryang/Desktop/application/',
    'buildtype=release'
  ]
)

# 获取选项值
someoption = get_option('someoption')
prefix = get_option('prefix')

if get_option('print')
  message('someoption', someoption)
endif

gtk_dep = dependency('gtk+-3.0')

# 头文件搜索目录
inc = include_directories('./include')

# 源文件列表
sources = ['main.c', './src/func.c']

# demo表示编译成可执行文件的名称，dependencies表示依赖的头文件路径和相关库文件
executable('main', sources, 
  include_directories: inc,
  install: true, # 是否安装
  install_dir: prefix, # 安装路径
  dependencies: gtk_dep)
