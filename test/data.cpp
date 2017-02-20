//  :copyright: (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.
//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test parsing known JSON dialects.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>


// TESTS
// -----


TEST(JsonOrg, Sections)
{
    std::vector<std::string> items = {
        "test/data/json.org/1.json",
        "test/data/json.org/2.json",
        "test/data/json.org/3.json",
        "test/data/json.org/4.json",
        "test/data/json.org/5.json",
    };

    for (const auto &item: items) {
        json::FileTextReader reader(item);
        do {
            ASSERT_NO_THROW(reader.read());
        } while (reader.isValid());
    }
}


TEST(TestSuite, SampleJson)
{
    json::FileTextReader reader("test/data/json_testsuite/sample.json");
    size_t counter = 0;
    do {
        ASSERT_NO_THROW(reader.read());
        if (reader.depth() == 1) {
            ++counter;
        }
    } while (reader.isValid());
    EXPECT_EQ(counter, 3);
}


TEST(Roundtrip, Sections)
{
    std::vector<std::string> items = {
        "test/data/json_roundtrip/roundtrip01.json",
        "test/data/json_roundtrip/roundtrip02.json",
        "test/data/json_roundtrip/roundtrip03.json",
        "test/data/json_roundtrip/roundtrip04.json",
        "test/data/json_roundtrip/roundtrip05.json",
        "test/data/json_roundtrip/roundtrip06.json",
        "test/data/json_roundtrip/roundtrip07.json",
        "test/data/json_roundtrip/roundtrip08.json",
        "test/data/json_roundtrip/roundtrip09.json",
        "test/data/json_roundtrip/roundtrip10.json",
        "test/data/json_roundtrip/roundtrip11.json",
        "test/data/json_roundtrip/roundtrip12.json",
        "test/data/json_roundtrip/roundtrip13.json",
        "test/data/json_roundtrip/roundtrip14.json",
        "test/data/json_roundtrip/roundtrip15.json",
        "test/data/json_roundtrip/roundtrip16.json",
        "test/data/json_roundtrip/roundtrip17.json",
        "test/data/json_roundtrip/roundtrip18.json",
        "test/data/json_roundtrip/roundtrip19.json",
        "test/data/json_roundtrip/roundtrip20.json",
        "test/data/json_roundtrip/roundtrip21.json",
        "test/data/json_roundtrip/roundtrip22.json",
        "test/data/json_roundtrip/roundtrip23.json",
        "test/data/json_roundtrip/roundtrip24.json",
        "test/data/json_roundtrip/roundtrip25.json",
        "test/data/json_roundtrip/roundtrip26.json",
        "test/data/json_roundtrip/roundtrip27.json",
        "test/data/json_roundtrip/roundtrip28.json",
        "test/data/json_roundtrip/roundtrip29.json",
        "test/data/json_roundtrip/roundtrip30.json",
        "test/data/json_roundtrip/roundtrip31.json",
        "test/data/json_roundtrip/roundtrip32.json",
    };

    for (const auto &item: items) {
        json::FileTextReader reader(item);
        do {
            ASSERT_NO_THROW(reader.read());
        } while (reader.isValid());
    }
}
