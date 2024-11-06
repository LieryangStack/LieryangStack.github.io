# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Li_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Li_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Liplugin_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Liplugin_autogen.dir\\ParseCache.txt"
  "Li_autogen"
  "Liplugin_autogen"
  )
endif()
