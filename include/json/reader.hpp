//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
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
    std::istream *stream_ = nullptr;
    std::deque<NodeType> node_;
    std::deque<size_t> offset_;
    std::string buffer_;
    ValueType type_;

public:
    TextReader() = delete;
    TextReader(const TextReader&) = delete;
    TextReader & operator=(const TextReader&) = delete;
    TextReader(TextReader&&) = default;
    TextReader & operator=(TextReader&&) = default;

    TextReader(std::istream &stream);

    // PROPERTIES
    std::istream & stream();
    const std::istream & stream() const;
    std::deque<NodeType> & node();
    const std::deque<NodeType> & node() const;
    std::deque<size_t> & offset();
    const std::deque<size_t> & offset() const;
    std::string & buffer();
    const std::string & buffer() const;
    ValueType & type();
    const ValueType & type() const;

    size_t depth() const;
    bool isEof() const;
    bool isBad() const;
    bool isValid() const;
    bool isEmpty() const;
    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;
    bool read();
};

}   /* json */
