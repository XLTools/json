//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Fuzz testing for JSON writers.
 */

#include <json/writer.hpp>
#include <gtest/gtest.h>


#ifdef __clang__


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    std::string string(reinterpret_cast<const char*>(Data), Size);
    try {
        json::StringTextWriter writer;
        writer.startArray();
        writer.write(string);
        writer.endArray();
    } catch (json::Exception) {
        // catching custom exceptions is fine.
    }

    return 0;
}


#endif          // __clang__
