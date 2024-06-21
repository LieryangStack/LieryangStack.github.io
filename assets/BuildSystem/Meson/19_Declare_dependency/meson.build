project('my_project', 'cpp', version: '1.0')

my_dep = dependency('my', 
                    allow_fallback: false,
                    required: true)

executable('demo', 'main.cpp',
           dependencies: my_dep)
