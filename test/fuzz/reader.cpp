//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Fuzz testing for JSON readers.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>


#ifdef __clang__


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    std::string string(reinterpret_cast<const char*>(Data), Size);
    try {
        json::StringTextReader reader(string);
    } catch (json::Exception) {
        // catching custom exceptions is fine.
    }

    return 0;
}


#endif          // __clang__
