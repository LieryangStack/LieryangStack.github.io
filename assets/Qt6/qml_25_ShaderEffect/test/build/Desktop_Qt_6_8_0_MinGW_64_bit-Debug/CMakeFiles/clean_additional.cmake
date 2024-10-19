# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\apptest_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\apptest_autogen.dir\\ParseCache.txt"
  "apptest_autogen"
  )
endif()
