# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appModelMapper_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appModelMapper_autogen.dir\\ParseCache.txt"
  "appModelMapper_autogen"
  )
endif()
