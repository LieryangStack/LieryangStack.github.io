# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\QQmlApplicationEngine_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\QQmlApplicationEngine_autogen.dir\\ParseCache.txt"
  "QQmlApplicationEngine_autogen"
  )
endif()
