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
        reader.read();
        for (volatile auto value: reader.array())
            ;
    };
    auto TEST_OBJECT = [](const std::string &data) {
        json::StringTextReader reader(data);
        reader.read();
        for (volatile auto value: reader.object())
            ;
    };

    // invalid
    ASSERT_THROW(TEST_ARRAY("[]"), json::ParserError);
    ASSERT_THROW(TEST_ARRAY("{}"), json::ParserError);
    ASSERT_THROW(TEST_OBJECT("[]"), json::ParserError);
    ASSERT_THROW(TEST_OBJECT("{}"), json::ParserError);

    // potentially valid
    ASSERT_THROW(TEST_ARRAY("{\"1\": {}}"), json::ParserError);
    TEST_OBJECT("{\"1\": {}}");
    TEST_ARRAY("{\"1\": []}");
    ASSERT_THROW(TEST_OBJECT("{\"1\": []}"), json::ParserError);
}
