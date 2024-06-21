project('my_project', 'cpp', version: '1.0')

my_dep = dependency('my', 
                    fallback: ['my', 'zlib_dep'],
                    required: true)

executable('demo', 'main.cpp',
           dependencies: my_dep)
