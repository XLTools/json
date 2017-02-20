//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test stream writer.
 */

#include <json/except.hpp>
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

    std::ostringstream buffer;
    json::TextWriter writer(buffer);
    writer.write(m);
    EXPECT_EQ(buffer.str(), "{1:2,3:4,5:6}");
}


TEST(TextWriter, NodelessInt)
{
    std::ostringstream buffer;
    json::TextWriter writer(buffer);
    ASSERT_THROW(writer.write(1), json::NodeError);
}


TEST(TextWriter, Int)
{
    std::ostringstream buffer;
    json::TextWriter writer(buffer);
    writer.startArray();
    writer.write(1);
    writer.endArray();
    EXPECT_EQ(buffer.str(), "[1]");
}


TEST(TextWriter, IntList)
{
    std::vector<int> v = {1};

    std::ostringstream buffer;
    json::TextWriter writer(buffer);
    writer.startArray();
    writer.write(1);
    writer.write(v);
    writer.endArray();
    EXPECT_EQ(buffer.str(), "[1,[1]]");
}


TEST(TextWriter, IntListDict)
{
    std::vector<int> v = {1};
    std::map<int, int> m = {
        {1, 2},
        {3, 4},
        {5, 6},
    };

    std::ostringstream buffer;
    json::TextWriter writer(buffer);
    writer.startArray();
    writer.write(1);
    writer.write(v);
    writer.write(m);
    writer.endArray();
    EXPECT_EQ(buffer.str(), "[1,[1],{1:2,3:4,5:6}]");
}
