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


TEST(JsonOrg, Section1)
{
    json::FileTextReader reader("test/data/json.org/1.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.isValid());
}


TEST(JsonOrg, Section2)
{
    json::FileTextReader reader("test/data/json.org/2.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.isValid());
}


TEST(JsonOrg, Section3)
{
    json::FileTextReader reader("test/data/json.org/3.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.isValid());
}


TEST(JsonOrg, Section4)
{
    json::FileTextReader reader("test/data/json.org/4.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.isValid());
}


TEST(JsonOrg, Section5)
{
    json::FileTextReader reader("test/data/json.org/5.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.isValid());
}
