project('my_project', 'cpp', version: '1.0')

conf_data = configuration_data()
conf_data.set('version', '1.2.3')
conf_data.set('TOKEN', '"token"')
conf_data.set('BAR', true, description : 'Set BAR if it is available')

# 有输入配置文件
configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf_data)

configure_file(output: 'config1.h',
               configuration: conf_data)


configure_file(output : 'config2.h',
  configuration : {
    'STRING' : '"foo"',
    'INT' : 42,
    'DEFINED' : true,
    'UNDEFINED' : false,
  }
)

executable('demo', 'main.cpp')
