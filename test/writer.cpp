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
    EXPECT_EQ(writer.str(), "{1:2,3:4,5:6}");
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
    writer.startArray();
    writer.write(1);
    writer.endArray();
    EXPECT_EQ(writer.str(), "[1]");
}


TEST(TextWriter, IntList)
{
    std::vector<int> v = {1};

    json::StringTextWriter writer;
    writer.startArray();
    writer.write(1);
    writer.write(v);
    writer.endArray();
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
    writer.startArray();
    writer.write(1);
    writer.write(v);
    writer.write(m);
    writer.endArray();
    EXPECT_EQ(writer.str(), "[1,[1],{1:2,3:4,5:6}]");
}
