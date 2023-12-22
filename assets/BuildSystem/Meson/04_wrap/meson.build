project('tutorial', 'cpp')

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
harfbuzz_dep   = dependency('harfbuzz', version: '>= 2.8.2',
                            fallback: ['harfbuzz', 'libharfbuzz_dep'],
                            default_options: ['coretext=enabled'])
executable('demo', ['main.cpp', 'sum.cpp'], include_directories: include_directories('.'), dependencies : gtest_dep, harfbuzz_dep)
