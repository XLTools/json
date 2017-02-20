#  :copyright: (c) 2015-2017 The Regents of the University of California.
#  :license: MIT, see licenses/mit.md for more details.

cmake_minimum_required(VERSION 3.4)
project(JSON CXX)

# FLAGS
# -----

set_property(GLOBAL PROPERTY CXX_STANDARD 17)
set(CXX_STANDARD TRUE)
if(UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    add_definitions(-fPIC)
endif()

option(BUILD_STATIC "Build static library" ON)

# LIBRARY
# -------

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES
    src/reader.cpp
)

if (BUILD_STATIC)
    add_library(JSON STATIC ${SOURCE_FILES})
else()
    add_library(JSON SHARED ${SOURCE_FILES})
endif()
