# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\qmlchartsgallery_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\qmlchartsgallery_autogen.dir\\ParseCache.txt"
  "qmlchartsgallery_autogen"
  )
endif()
