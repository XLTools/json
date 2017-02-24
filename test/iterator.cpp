//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Check iterators in heterogenous data.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>


// HELPERS
// -------

typedef std::unordered_set<std::string> Set;


static void TEST_ARRAY(const std::string &data,
    const Set &expected)
{
    Set actual;
    json::StringTextReader reader(data);
    for (auto value: reader.array()) {
        actual.emplace(std::string(value));
    }
    EXPECT_EQ(actual, expected);
}


static void TEST_OBJECT(const std::string &data,
    const Set &expected)
{
    Set actual;
    json::StringTextReader reader(data);
    for (auto pair: reader.object()) {
        actual.emplace(std::string(pair.first));
    }
    EXPECT_EQ(actual, expected);
}


static void TEST_NESTED_ARRAY(const std::string &data,
    const Set &expected)
{
    Set actual;
    json::StringTextReader reader(data);
    for (auto outer: reader.array()) {
        for (auto inner: reader.array())
            actual.emplace(std::string(inner));
    }
    EXPECT_EQ(actual, expected);
}


static void TEST_NESTED_OBJECT(const std::string &data,
    const Set &outer,
    const Set &inner)
{
    Set outer_, inner_;
    json::StringTextReader reader(data);
    for (auto pair: reader.object()) {
        outer_.emplace(std::string(pair.first));
        for (auto value: reader.array())
            inner_.emplace(std::string(value));
    }
    EXPECT_EQ(outer_, outer);
    EXPECT_EQ(inner_, inner);
}


// TESTS
// -----


TEST(Iterators, Homogenous)
{
    TEST_ARRAY("[1, 2, 3, 4, 5]", Set({"1", "2", "3", "4", "5"}));
    ASSERT_THROW(TEST_OBJECT("[1, 2, 3, 4, 5]", Set({"1", "2", "3", "4", "5"})), json::ParserError);
    TEST_OBJECT("{\"1\": 2, \"3\": 4}", Set({"1", "3"}));
    ASSERT_THROW(TEST_ARRAY("{\"1\": 2, \"3\": 4}", Set({"1", "3"})), json::ParserError);
}


TEST(Iterators, HeterogenousEnding)
{
    TEST_ARRAY("[1, 2, 3, 4, [5, 6, 7]]", Set({"1", "2", "3", "4", ""}));
    TEST_ARRAY("[1, 2, 3, [8, 9], [5, 6, 7]]", Set({"1", "2", "3", ""}));
    ASSERT_THROW(TEST_OBJECT("[1, 2, 3, 4, [5, 6, 7]]", Set({"1", "2", "3", "4", ""})), json::ParserError);
    ASSERT_THROW(TEST_OBJECT("[1, 2, 3, [8, 9], [5, 6, 7]]", Set({"1", "2", "3", ""})), json::ParserError);

    TEST_OBJECT("{\"1\": 2, \"3\": [5, 6], \"4\": [9, 8]}", Set({"1", "3", "4"}));
    ASSERT_THROW(TEST_ARRAY("{\"1\": 2, \"3\": [5, 6], \"4\": [9, 8]}", Set({"1", "3", "4"})), json::ParserError);
}


TEST(Iterators, HeterogenousBeginning)
{
    TEST_ARRAY("[1, 2, 3, 4, [6, 7, 8], 5]", Set({"1", "2", "3", "4", "5", ""}));
    ASSERT_THROW(TEST_OBJECT("[1, 2, 3, 4, [6, 7, 8], 5]", Set({"1", "2", "3", "4", "5", ""})), json::ParserError);

    TEST_OBJECT("{\"1\": 2, \"3\": [4, 5], \"6\": [], \"7\": [8, 9], \"10\": 11}", Set({"1", "3", "6", "7", "10"}));
    ASSERT_THROW(TEST_ARRAY("{\"1\": 2, \"3\": [4, 5], \"6\": [], \"7\": [8, 9], \"10\": 11}", Set({"1", "3", "6", "7", "10"})), json::ParserError);
}


TEST(Iterators, NestedIteration)
{
    TEST_NESTED_ARRAY("[[1, 2], [3, 4], [5, 6]]", Set({"1", "2", "3", "4", "5", "6"}));
    TEST_NESTED_OBJECT("{\"1\": [2, 3], \"4\": [5, 6]}", Set({"1", "4"}), Set({"2", "3", "5", "6"}));
}
