#  :copyright: (c) 2015-2017 The Regents of the University of California.
#  :license: MIT, see licenses/mit.md for more details.

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

if(BUILD_STATIC)
    add_library(JSON STATIC ${SOURCE_FILES})
else()
    add_library(JSON SHARED ${SOURCE_FILES})
endif()

# EXAMPLES
# --------

if(BUILD_TESTS)
    add_executable(JSONSimpleWriter example/simplewriter.cpp)
    target_link_libraries(JSONSimpleWriter JSON)

    add_executable(JSONSimpleReader example/simplereader.cpp)
    target_link_libraries(JSONSimpleReader JSON)
endif()

# TESTS
# -----

set(TEST_FILES
    test/main.cpp
    test/benchmark.cpp
    test/checker.cpp
    test/data.cpp
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
    )

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
    add_executable(JSONFuzzWriter test/fuzz/writer.cpp)

    target_link_libraries(JSONFuzzReader LLVMFuzzer JSON Threads::Threads)
    target_link_libraries(JSONFuzzWriter LLVMFuzzer JSON Threads::Threads)
endif()
