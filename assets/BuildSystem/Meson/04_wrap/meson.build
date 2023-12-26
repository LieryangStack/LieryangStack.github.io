project('tutorial', 'cpp')

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
executable('demo', ['main.cpp', 'sum.cpp'], include_directories: include_directories('.'), dependencies : gtest_dep)
