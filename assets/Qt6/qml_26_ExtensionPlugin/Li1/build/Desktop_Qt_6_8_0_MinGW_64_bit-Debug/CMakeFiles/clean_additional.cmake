# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Li1_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Li1_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\Li1plugin_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Li1plugin_autogen.dir\\ParseCache.txt"
  "Li1_autogen"
  "Li1plugin_autogen"
  )
endif()
