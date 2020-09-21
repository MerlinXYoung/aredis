cmake_minimum_required(VERSION 3.1)
project(AREDIS)
# -D__AREDIS_SSL__
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(hiredis REQUIRED)
find_package(Threads REQUIRED)
find_package(Libuv REQUIRED)
include_directories(${hiredis_INCLUDE_DIRS})
#include_directories(hiredis::hiredis)
include_directories(Libuv::Libuv)


# file(GLOB SRCS "tutorial/*.cpp")
set(SRCS "tutorial/uv.cpp")
set(LINK_ARGS hiredis::hiredis Libuv::Libuv Threads::Threads)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
foreach(src ${SRCS})
    string(REGEX REPLACE ".*/" "" var ${src})
    string(REGEX REPLACE ".cpp" "" tgt ${var})
    message("${src}")
    add_executable(${tgt} ${src})
    target_link_libraries(${tgt} ${LINK_ARGS} )
endforeach(var)
