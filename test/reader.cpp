//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Test stream reader.
 */

#include <json/except.hpp>
#include <json/reader.hpp>
#include <gtest/gtest.h>

#include <sstream>


// TESTS
// -----


TEST(TextReader, ReadInt)
{
    std::istringstream buffer("[1]");
    json::TextReader reader(buffer);
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY);
    EXPECT_EQ(reader.depth(), 1);

    reader.read();
    EXPECT_EQ(reader.type(), json::ValueType::NUMBER);
    EXPECT_EQ(reader.depth(), 1);
    EXPECT_EQ(reader.buffer(), "1");

    reader.read();
    EXPECT_EQ(reader.depth(), 0);
    EXPECT_FALSE(reader.isValid());
}


TEST(TextReader, ReadLeadingComma)
{
    std::istringstream buffer("[,1]");
    json::TextReader reader(buffer);
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY);

    ASSERT_THROW(reader.read(), json::ParserError);
}


TEST(TextReader, ReadTrailingComma)
{
    std::istringstream buffer("[1,]");
    json::TextReader reader(buffer);
    EXPECT_EQ(reader.type(), json::ValueType::ARRAY);

    reader.read();
    ASSERT_THROW(reader.read(), json::ParserError);
}

