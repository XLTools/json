//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  Simple example for how to use the TextWriter API.
 */

#include "json/writer.hpp"

#include <cmath>
#include <iostream>


int main(void)
{
    // use FileTextWriter to write to a file-handle.
    // use TextWriter to inherit an existing `std::ostream`.
    json::StringTextWriter writer;

    // create a root map
    writer.start_object();
    for (size_t i = 0; i < 5; ++i) {
        writer.write(i, static_cast<size_t>(std::pow(i, 2)));
    }
    // directly write a nested array
    std::vector<std::string> v = {"1", "2", "3", "4", "5"};
    writer.write("array", v);

    writer.end_object();

    std::cout << "------------------------------------------------------------------\n"
              << "                        Expected output:                          \n"
              << " {\"0\":0,\"1\":1,\"2\":4,\"3\":9,\"4\":16,\"array\":[\"1\",\"2\",\"3\",\"4\",\"5\"]}  \n"
              << "-------------------------------------------------------\n"
              << "                         Actual output:                           \n"
              << " " << writer.str() << "  \n"
              << "------------------------------------------------------------------\n"
              << std::endl;

    return 0;
}
