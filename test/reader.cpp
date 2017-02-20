//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test stream reader.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>

#include <sstream>


// TESTS
// -----


TEST(TextReader, ReadInt)
{
    json::StringTextReader reader("[1]");
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY_START);
    EXPECT_EQ(reader.depth(), 1);

    reader.read();
    EXPECT_EQ(reader.type(), json::ValueType::NUMBER);
    EXPECT_EQ(reader.depth(), 1);
    EXPECT_EQ(reader.value(), "1");
    EXPECT_EQ(reader.value<int>(), 1);

    reader.read();
    EXPECT_EQ(reader.depth(), 0);
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY_END);
    EXPECT_FALSE(reader.isValid());
}


TEST(TextReader, ReadLeadingComma)
{
    json::StringTextReader reader("[,1]");
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY_START);

    ASSERT_THROW(reader.read(), json::ParserError);
}


TEST(TextReader, ReadTrailingComma)
{
    json::StringTextReader reader("[1,]");
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY_START);

    reader.read();
    ASSERT_THROW(reader.read(), json::ParserError);
}

