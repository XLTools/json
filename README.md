Json++
======

Json++ is a memory-efficient, dependency-free, lazy C++11 JSON reader and writer.

**Table of Contents**

- [Motivation](#motivation)
- [Design](#design)
- [Getting Started](#getting-started)
- [Building](#building)
- [Testing](#testing)

## Motivation

Most parsers provide one of two interfaces: a DOM API, which parses the entire document tree, and a SAX API, which provides event-based handling for lazy parsing. Although DOM parsers are easy-to-use, they scale poorly with large documents. SAX parsers, on the other hand, require large amounts of boilerplate for basic tasks, making it difficult to do complex processing.

The .NET library took a different approach, and designed push/pull-based APIs with a TextReader and TextWriter approach: the API iterates over nodes in the document, allowing you to process data at each step. Json++ makes a text-reader like API in native C++ for high-performance, save C++ parsing.

## Design

## Getting Started

```cpp
#include <json/reader.hpp>
#include <iostream>


int main(int argc, char *argv[])
{
// TODO: i need a simple API to check if isStart
    json::StringTextReader reader("{5:3,1:2,7:{6:2}}");
    reader.read();
}
```

## Building

Simply clone, configure with CMake, and build.

```bash
git clone https://github.com/Alexhuszagh/json.git
cd json/build
cmake .. -_DBUILD_TESTS=ON      # "-DBUILD_FUZZ=ON" for fuzzing with Clang
make -j 5                       # "msbuild JSON.sln" for MSVC
```

## Testing

It has been tested with a variety of JSON files, for compliance and accurate parsing, fully passing the [conformance tests](https://github.com/miloyip/nativejson-benchmark/tree/master/data/jsonchecker) and [roundtrip tests](https://github.com/miloyip/nativejson-benchmark/tree/master/data/roundtrip) from Milo Yip's JSON benchmarks.

The core parser has also been extensively fuzzed (see [Fuzz Tests](/test/fuzz)), since bugs in JSON parsing can be used as attack vectors against web services.

## Portability

Json++ has been tested with the following compilers and operating systems:

- 64-bit Linux with Clang 3.8.2
- 64-bit Linux with GCC 5.4.0
- 32/64-bit Windows with MinGW 5.3.0 (MXE, MinGW, and MSYS2) 
//- 32/64-bit Windows with Visual Studio 14 2015

## Documentation

Coming soon, for now, see the the [examples](/example) for how to use Json++.

## Planned Features

- DOM-style API using the TextReader internally.
- Pretty Printing

## Contributors

- Alex Huszagh

Json++ also uses some code and test files from Niels Lohmann and Milo Yip, from their [Json](https://github.com/nlohmann/json) and [RapidJson](https://github.com/miloyip/rapidjson/) projects. All files in the [jsonchecker](/test/data/jsonchecker) directory are from Milo Yip's [nativejson-benchmark](https://github.com/miloyip/nativejson-benchmark/tree/master/data/roundtrip) and the remaining sample files come from Niel Lohmann's JSON tests.

## Contributor Guidelines

All useful pull requests will be merged, provided that they do not add external dependencies and follow these guidelines.

1. Preprocessor macros should be used sparingly.
2. Code syntax should not depend on the preprocessor.
3. Your code must be readable.

## License

MIT, see [license](LICENSE.md).
