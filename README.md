Json++
======

Json++ is a memory-efficient, dependency-free, lazy C++11 JSON reader and writer.

**Table of Contents**

- [Motivation](#motivation)
- [Getting Started](#getting-started)
- [Design](#design)
- [Building](#building)
- [Testing](#testing)
- [Portability](#portability)
- [Documentation](#documentation)
- [Planned Features](#planned-features)
- [Contributors](#contributors)
- [Contributors Guidelines](#contributors-guidelines)
- [License](#license)

## Motivation

**DOM sucks.** DOM APIs read the entire document into memory, making DOM useless for large workloads.

**SAX sucks.** SAX APIs map specific events to specific handlers, requiring conditional branches for each tree in the document, producing tedious amounts of boilerplate.

**Iteration rules.** Inspired by the TextReader and TextWriter APIs from .NET, Json++ provides iterative access to each node as the document is parsed, simplifying tree-specific parsing with minimal overhead. 

Json++ features:

- Low memory overhead
- Tree-like logic
- STL-like iterators, auto-ranges, and STL container adapters

## Getting Started

```cpp
#include <json/reader.hpp>
#include <json/writer.hpp>
#include <iostream>


int main(void)
{
    // read document into map
    json::StringTextReader reader(" {\"1\":2}  \n");
    std::unordered_map<int, int> map(reader.object());

    // re-create JSON document from map 
    json::StringTextWriter writer;
    writer.write(map);

    // write {"1":2} to stdout
    std::cout << writer.str() << std::endl;

    return 0;
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
- Fast string formatting and extraction (replacing std::to_string and std::stod).

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
