# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\StudyQQmlComponent_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\StudyQQmlComponent_autogen.dir\\ParseCache.txt"
  "StudyQQmlComponent_autogen"
  )
endif()
