# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\stringlistmodelexample_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\stringlistmodelexample_autogen.dir\\ParseCache.txt"
  "stringlistmodelexample_autogen"
  )
endif()
