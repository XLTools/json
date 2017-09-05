#  :copyright: (c) 2015-2017 The Regents of the University of California.
#  :license: MIT, see LICENSE.md for more details.

cmake_minimum_required(VERSION 3.4)
project(JSON CXX)

# FLAGS
# -----

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD TRUE)

option(BUILD_STATIC "Build static library" ON)
option(BUILD_TESTS "Build JSON library tests" OFF)
option(BUILD_FUZZ "Build fuzz testing" OFF)
option(BUILD_EXAMPLES "Build example files" OFF)

if(BUILD_STATIC)
    if(UNIX)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
        add_definitions(-fPIC)
    elseif(MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} /MTd /O0")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Ox")
    endif()
endif()

# DEPENDENCIES
# ------------

if(NOT TARGET lexi)
    add_subdirectory(lexi)
endif()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lexi/include")

# LIBRARY
# -------

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCE_FILES
    src/except.cpp
    src/iterator.cpp
    src/reader.cpp
    src/view.cpp
    src/writer.cpp
)

if(BUILD_STATIC)
    add_library(JSON STATIC ${SOURCE_FILES})
else()
    add_library(JSON SHARED ${SOURCE_FILES})
endif()
set_target_properties(JSON PROPERTIES OUTPUT_NAME json)

# EXAMPLES
# --------

if(BUILD_EXAMPLES)
    add_executable(JSONSimpleWriter example/simplewriter.cpp)
    target_link_libraries(JSONSimpleWriter JSON Lexi)
    set_target_properties(JSONSimpleWriter PROPERTIES OUTPUT_NAME json_simplewriter)

    add_executable(JSONSimpleReader example/simplereader.cpp)
    target_link_libraries(JSONSimpleReader JSON Lexi)
    set_target_properties(JSONSimpleReader PROPERTIES OUTPUT_NAME json_simplereader)

    add_executable(JSONRange example/range.cpp)
    target_link_libraries(JSONRange JSON Lexi)
    set_target_properties(JSONRange PROPERTIES OUTPUT_NAME json_range)

    add_executable(JSONStl example/stl.cpp)
    target_link_libraries(JSONStl JSON Lexi)
    set_target_properties(JSONStl PROPERTIES OUTPUT_NAME json_stl)
endif()

# TESTS
# -----

set(TEST_FILES
    test/main.cpp
    test/benchmark.cpp
    test/checker.cpp
    test/data.cpp
    test/iterator.cpp
    test/overrun.cpp
    test/rfc7419.cpp
    test/reader.cpp
    test/writer.cpp
)

if(BUILD_TESTS)
    if(NOT TARGET gtest)
        add_subdirectory(googletest)
    endif()
    add_executable(JSON_TESTS ${TEST_FILES})
    target_link_libraries(JSON_TESTS
        gtest
        gtest_main
        JSON
        Lexi
    )
    set_target_properties(JSON_TESTS PROPERTIES OUTPUT_NAME test)

     add_custom_target(check_json
        COMMAND $<TARGET_FILE:JSON_TESTS>
        DEPENDS JSON_TESTS
    )
endif()


if(BUILD_FUZZ AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=bool -fsanitize-coverage=edge,8bit-counters,trace-cmp")
    if(NOT TARGET LLVMFuzzer)
        add_subdirectory(Fuzzer)
    endif()

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)

    add_executable(JSONFuzzReader test/fuzz/reader.cpp)
    target_link_libraries(JSONFuzzReader LLVMFuzzer JSON Lexi Threads::Threads)
    set_target_properties(JSONFuzzReader PROPERTIES OUTPUT_NAME json_fuzzreader)

    add_executable(JSONFuzzWriter test/fuzz/writer.cpp)
    target_link_libraries(JSONFuzzWriter LLVMFuzzer JSON Lexi Threads::Threads)
    set_target_properties(JSONFuzzWriter PROPERTIES OUTPUT_NAME json_fuzzwriter)
endif()
