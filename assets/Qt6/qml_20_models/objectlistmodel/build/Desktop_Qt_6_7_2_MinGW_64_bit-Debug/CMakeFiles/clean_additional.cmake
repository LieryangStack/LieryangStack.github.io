# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\objectlistmodelexample_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\objectlistmodelexample_autogen.dir\\ParseCache.txt"
  "objectlistmodelexample_autogen"
  )
endif()
