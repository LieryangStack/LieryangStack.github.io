project('my_project', 'cpp', version: '1.0')

libsimple_dep = dependency('simple')
gtk_dep = dependency('gtk+-3.0')


executable('demo',
           'main.cpp',
           dependencies : [libsimple_dep, gtk_dep],
           install : true)
