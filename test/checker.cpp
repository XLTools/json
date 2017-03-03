//  :copyright: (c) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test compliance with known JSON files.
 */


#include <json/reader.hpp>
#include <gtest/gtest.h>


// TESTS
// -----


TEST(JsonChecker, Pass)
{
    std::vector<std::string> items = {
        "test/data/jsonchecker/pass01.json",
        "test/data/jsonchecker/pass02.json",
        "test/data/jsonchecker/pass03.json",
    };

    for (const auto &item: items) {
        json::FileTextReader reader(item);
        do {
            ASSERT_NO_THROW(reader.read());
        } while (reader.is_valid());
    }
}


TEST(JsonChecker, Fail)
{
    std::vector<std::string> items = {
        "test/data/jsonchecker/fail02.json",
        "test/data/jsonchecker/fail03.json",
        "test/data/jsonchecker/fail04.json",
        "test/data/jsonchecker/fail05.json",
        "test/data/jsonchecker/fail06.json",
        "test/data/jsonchecker/fail07.json",
        "test/data/jsonchecker/fail08.json",
        "test/data/jsonchecker/fail09.json",
        "test/data/jsonchecker/fail10.json",
        "test/data/jsonchecker/fail11.json",
        "test/data/jsonchecker/fail12.json",
        "test/data/jsonchecker/fail13.json",
        "test/data/jsonchecker/fail14.json",
        "test/data/jsonchecker/fail15.json",
        "test/data/jsonchecker/fail16.json",
        "test/data/jsonchecker/fail17.json",
        "test/data/jsonchecker/fail19.json",
        "test/data/jsonchecker/fail20.json",
        "test/data/jsonchecker/fail21.json",
        "test/data/jsonchecker/fail22.json",
        "test/data/jsonchecker/fail23.json",
        "test/data/jsonchecker/fail24.json",
        "test/data/jsonchecker/fail25.json",
        "test/data/jsonchecker/fail26.json",
        "test/data/jsonchecker/fail27.json",
        "test/data/jsonchecker/fail28.json",
        "test/data/jsonchecker/fail29.json",
        "test/data/jsonchecker/fail30.json",
        "test/data/jsonchecker/fail31.json",
        "test/data/jsonchecker/fail32.json",
        "test/data/jsonchecker/fail33.json",
    };

    auto TEST_FAIL = [](const std::string &path) {
        json::FileTextReader reader(path);
        do {
            reader.read();
        } while (reader.is_valid());
    };

    for (const auto &item: items) {
        try {
            TEST_FAIL(item);
        } catch (json::Exception) {
            continue;
        }
        std::cout << item << std::endl;
        EXPECT_FALSE(true);
    }
}
