# CMakeLists.txt
cmake_minimum_required (VERSION 3.10.2)
project (set_test)
 
set (cache_entry_val ON OFF CACHE BOOL "choose ON to enable")
message (">>> value = ${cache_entry_val}")
 
set (cache_entry_val2 ON CACHE BOOL "choose ON to enable" FORCE)
message (">>> value2 = ${cache_entry_val2}")
 
set (cache_entry_val3 ON)
set (cache_entry_val3 OFF CACHE BOOL "choose ON to enable")
message (">>> value3 = ${cache_entry_val3}")
 
set (cache_entry_input OFF CACHE BOOL "choose ON to enable")
message (">>> value4 = ${cache_entry_input}")
 
set (mypath "test" CACHE FILEPATH "choose a file path")
message (">>> value5 = ${mypath}")