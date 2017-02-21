//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  Basic read/write example for the TextReader/TextWriter API.
 */

#include "json/reader.hpp"
#include "json/writer.hpp"

#include <iostream>


int main(void)
{
    // READER
    // ------

    std::unordered_map<int, int> map;

    // iterate over the child nodes
    json::StringTextReader reader(" {\"1\":2}  \n");
    for (const auto &pair: reader.object()) {
        map[int(pair.first)] = int(pair.second);
    }

    // WRITER
    // ------
    json::StringTextWriter writer;
    writer.write(map);

    // write {1:2} to stdout
    std::cout << writer.str() << std::endl;

    return 0;
}
