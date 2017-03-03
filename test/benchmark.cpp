//  :copyright: (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.
//  :copyright: (c) 2015-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/*
 *  \addtogroup Tests
 *  \brief Rudimentary benchmark for the JSON reader.
 */

#include <json/reader.hpp>
#include <gtest/gtest.h>

#include <chrono>


// TESTS
// -----


TEST(Jeopardy, Benchmark)
{
    typedef std::chrono::high_resolution_clock Clock;

    auto t1 = Clock::now();
    json::FileTextReader reader("test/data/jeopardy.json");
    do {
        ASSERT_NO_THROW(reader.read());
    } while (reader.is_valid());
    auto t2 = Clock::now();

    std::cout << "Delta t2-t1: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << " nanoseconds" << std::endl;
}
