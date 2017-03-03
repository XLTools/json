//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test stream writer.
 */

#include <json/writer.hpp>
#include <gtest/gtest.h>

#include <sstream>


// TESTS
// -----


TEST(TextWriter, WriteMap)
{
    std::map<int, int> m = {
        {1, 2},
        {3, 4},
        {5, 6},
    };

    json::StringTextWriter writer;
    writer.write(m);
    EXPECT_EQ(writer.str(), "{\"1\":2,\"3\":4,\"5\":6}");
}


TEST(TextWriter, NodelessInt)
{
    json::StringTextWriter writer;
    writer.write(1);
    EXPECT_EQ(writer.str(), "1");
}


TEST(TextWriter, Int)
{
    json::StringTextWriter writer;
    writer.start_array();
    writer.write(1);
    writer.end_array();
    EXPECT_EQ(writer.str(), "[1]");
}


TEST(TextWriter, IntList)
{
    std::vector<int> v = {1};

    json::StringTextWriter writer;
    writer.start_array();
    writer.write(1);
    writer.write(v);
    writer.end_array();
    EXPECT_EQ(writer.str(), "[1,[1]]");
}


TEST(TextWriter, IntListDict)
{
    std::vector<int> v = {1};
    std::map<int, int> m = {
        {1, 2},
        {3, 4},
        {5, 6},
    };

    json::StringTextWriter writer;
    writer.start_array();
    writer.write(1);
    writer.write(v);
    writer.write(m);
    writer.end_array();
    EXPECT_EQ(writer.str(), "[1,[1],{\"1\":2,\"3\":4,\"5\":6}]");
}


TEST(TextWriter, NestedKeys)
{
    json::StringTextWriter writer;
    writer.start_object();
    writer.write_key(1);
    writer.start_object();
    writer.end_object();
    writer.end_object();
    EXPECT_EQ(writer.str(), "{\"1\":{}}");
}


TEST(TextWriter, SeparatedKeyValue)
{
    json::StringTextWriter writer;
    writer.start_object();
    writer.write_key(1);
    writer.write_value(2);
    writer.write_key(3);
    writer.write_value(4);
    writer.end_object();
    EXPECT_EQ(writer.str(), "{\"1\":2,\"3\":4}");
}


TEST(TextWriter, ImproperClose)
{
    json::StringTextWriter writer;
    writer.start_object();
    writer.write_key(1);
    ASSERT_THROW(writer.end_object(), json::NodeError);
}


TEST(TextWriter, ImproperKey)
{
    json::StringTextWriter writer;
    writer.start_array();
    ASSERT_THROW(writer.write_key(1), json::NodeError);
}


TEST(TextWriter, DuplicateKeys)
{
    json::StringTextWriter writer;
    writer.start_object();
    writer.write_key(1);
    ASSERT_THROW(writer.write_key(1), json::NodeError);
}
