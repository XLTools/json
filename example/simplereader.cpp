//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  Simple example for how to use the TextReader API.
 */

#include "json/reader.hpp"

#include <iostream>


int main(void)
{
    // use FileTextReaderto write to a file-handle.
    // use TextReader to inherit an existing `std::istream`.
    json::StringTextReader reader(" {\"0\":0,\"1\":1,\"2\":4,\"3\":9,\"4\":16,\"array\":[\"1\",\"2\",\"3\",\"4\",\"5\"]}  \n");

    do {
        reader.read();
        if (reader.hasKey() && reader.hasValue()) {
            std::cout << "Have key-value pair of {\"" << reader.key()
                      << "\" : \"" << reader.value() << "\"} at a depth of "
                      << reader.depth() << "\n";
        } else if (reader.hasKey()) {
            std::cout << "Initializing child array with key \""
                      << reader.key() << "\" at a depth of "
                      << reader.depth() << "\n";
        } else if (reader.hasValue()) {
            std::cout << "Reading array value of \"" << reader.value()
                      << "\" at a depth of " << reader.depth() << "\n";
        }
    } while (reader.isValid());

    return 0;
}
