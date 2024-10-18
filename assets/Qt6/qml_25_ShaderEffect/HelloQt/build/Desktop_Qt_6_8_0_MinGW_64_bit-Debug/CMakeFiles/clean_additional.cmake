# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appHelloQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appHelloQt_autogen.dir\\ParseCache.txt"
  "appHelloQt_autogen"
  )
endif()
