//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup Json
 *  \brief Public header for JSON TextReader.
 */

#pragma once

#include "node.hpp"

#include <deque>
#include <istream>


namespace json
{
// OBJECTS
// -------


/** \brief
 */
class TextReader
{
protected:
    std::istream *stream = nullptr;
    std::deque<NodeType> node;

public:
    TextReader() = delete;
    TextReader(const TextReader&) = delete;
    TextReader & operator=(const TextReader&) = delete;
    TextReader(TextReader&&) = default;
    TextReader & operator=(TextReader&&) = default;

    // PROPERTIES
    size_t depth() const;
    bool isEof() const;
    bool isBad() const;
    bool isValid() const;
    bool isArray() const;
    bool isObject() const;
};

}   /* json */
