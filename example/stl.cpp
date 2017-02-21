//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  Read/write example using STL-container helpers.
 */

#include "json/reader.hpp"
#include "json/writer.hpp"

#include <iostream>


int main(void)
{
    // read document into map
    json::StringTextReader reader(" {\"1\":2}  \n");
    std::unordered_map<int, int> map(reader.object());

    // write JSON document from map
    json::StringTextWriter writer;
    writer.write(map);

    // write {"1":2} to stdout
    std::cout << writer.str() << std::endl;

    return 0;
}
