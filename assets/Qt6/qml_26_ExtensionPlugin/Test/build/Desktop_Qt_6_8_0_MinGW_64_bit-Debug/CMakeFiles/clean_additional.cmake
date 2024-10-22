# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ExampleProject_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ExampleProject_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Test_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Test_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Testplugin_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Testplugin_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Testplugin_init_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Testplugin_init_autogen.dir\\ParseCache.txt"
  "ExampleProject_autogen"
  "Test_autogen"
  "Testplugin_autogen"
  "Testplugin_init_autogen"
  )
endif()
