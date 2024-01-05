project('my_project', 'cpp', version: '1.0')

# 执行子构建
subdir('libsimple')

gtk_dep = dependency('gtk+-3.0')


# 子构建的 libsimple_dep 可以直接使用
executable('demo',
           'main.cpp',
           dependencies : [libsimple_dep, gtk_dep],
           install : true)
