//  :copyright: (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.
//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test parsing RFC7419 JSON data.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>


// TESTS
// -----


TEST(Rfc7419, Strings)
{
    auto TEST_STRING = [](const std::string &data, const std::string &expected) {
        json::StringTextReader reader(data);
        EXPECT_EQ(reader.buffer()[1], expected);
    };

    TEST_STRING("\"\\u005C\"", "\\");
    TEST_STRING("\"\\uD834\\uDD1E\"", "𝄞");
    TEST_STRING("\"𝄞\"", "𝄞");
}


TEST(Rfc7419, Doubles)
{
    auto TEST_DOUBLE = [](const std::string &data, const double expected) {
        json::StringTextReader reader(data);
        //EXPECT_EQ(reader.buffer()[1], expected);
    };
}
