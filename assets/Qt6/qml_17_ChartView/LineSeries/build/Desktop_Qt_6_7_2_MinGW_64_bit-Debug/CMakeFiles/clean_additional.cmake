# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appLineSeries_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appLineSeries_autogen.dir\\ParseCache.txt"
  "appLineSeries_autogen"
  )
endif()
