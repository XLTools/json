//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Intentionally overrun our source buffer.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>


// TESTS
// -----


TEST(Overrun, SampleArray)
{
    auto TEST_ARRAY = [](const std::string &data) {
        json::StringTextReader reader(data);
        // make sure the loop isn't optimized
        for (volatile auto value: reader.array());
    };

    TEST_ARRAY("{}");
}
