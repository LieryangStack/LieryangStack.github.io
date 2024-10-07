# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\chartsgallery_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\chartsgallery_autogen.dir\\ParseCache.txt"
  "chartsgallery_autogen"
  )
endif()
