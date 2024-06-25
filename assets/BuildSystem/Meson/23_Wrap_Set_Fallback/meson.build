project('my_project', 'cpp', version: '1.0')

my_dep = dependency('my_zlib', required: true, fallback: ['my_zlib', 'zlib_dep'])

executable('demo', 'main.cpp',
           dependencies: my_dep)
