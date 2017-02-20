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
option(BUILD_TESTS "Build JSON library tests" OFF)

# LIBRARY
# -------

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES
    src/detail/ieee754.cpp
    src/detail/string.cpp
    src/detail/write.cpp
    src/except.cpp
    src/reader.cpp
    src/writer.cpp
)

if (BUILD_STATIC)
    add_library(JSON STATIC ${SOURCE_FILES})
else()
    add_library(JSON SHARED ${SOURCE_FILES})
endif()

# TESTS
# -----

set(TEST_FILES
    test/main.cpp
    test/benchmark.cpp
    test/data.cpp
    test/rfc7419.cpp
    test/reader.cpp
    test/writer.cpp
)

if (BUILD_TESTS)
    if(NOT TARGET gtest)
        add_subdirectory(googletest)
    endif()
    add_executable(JSON_TESTS ${TEST_FILES})
    target_link_libraries(JSON_TESTS
        gtest
        gtest_main
        JSON
    )

     add_custom_target(check_json
        COMMAND $<TARGET_FILE:JSON_TESTS>
        DEPENDS JSON_TESTS
    )
endif()
