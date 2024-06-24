project('my_project', 'cpp', version: '1.0')

my_dep = dependency('zlib-1.0', 
                    required: true)

executable('demo', 'main.cpp',
           dependencies: my_dep)
