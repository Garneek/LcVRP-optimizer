cmake_minimum_required(VERSION 4.0)

project (LcVRP_optimizer)

set(CMAKE_CXX_STANDARD 17)

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES INCLUDE_DIRS)

add_executable(optimizer ${SOURCES})

include_directories(${INCLUDE_DIRS})
include_directories(src/)

target_include_directories(optimizer PRIVATE ${INCLUDE_DIRS})