# 'tutorial' 整个项目的名称， `c`编程语言是c语言， version表示该项目的版本
project('tutorial', 'c', version: '1.0')
gtk_dep = dependency('gtk+-3.0')
# demo表示编译成可执行文件的名称，dependencies表示依赖的头文件路径和相关库文件
executable('demo', 'main.c', dependencies: gtk_dep)

