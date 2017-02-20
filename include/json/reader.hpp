//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Public header for JSON TextReader.
 */

#pragma once

#include "except.hpp"
#include "node.hpp"
#include "detail/extract.hpp"

#include <array>
#include <deque>
#include <fstream>
#include <istream>
#include <sstream>


namespace json
{
// OBJECTS
// -------


/** \brief Base text reader for JSON parsing.
 */
class TextReader
{
protected:
    std::istream *stream_ = nullptr;
    std::deque<NodeType> node_;
    std::deque<size_t> offset_;
    std::array<std::string, 2> buffer_;
    ValueType type_;

    TextReader() = default;
    void open(std::istream &stream);

public:
    TextReader(const TextReader&) = delete;
    TextReader & operator=(const TextReader&) = delete;
    TextReader(TextReader&&) = default;
    TextReader & operator=(TextReader&&) = default;

    TextReader(std::istream &stream);

    // INTERNAL
    std::istream & stream();
    const std::istream & stream() const;
    std::deque<NodeType> & node();
    const std::deque<NodeType> & node() const;
    std::deque<size_t> & offset();
    const std::deque<size_t> & offset() const;
    std::array<std::string, 2> & buffer();
    const std::array<std::string, 2> & buffer() const;
    ValueType & type();
    const ValueType & type() const;

    // DATA
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

    // EXTRACT
    template <typename T = std::string>
    T key() const;

    template <typename T = std::string>
    T value() const;
};


/** \brief Text reader for file-backed JSON stores.
 */
class FileTextReader: public TextReader
{
protected:
    std::ifstream fstream;

public:
    FileTextReader(const std::string &path);
};


/** \brief Text reader for string-based JSON stores.
 */
class StringTextReader: public TextReader
{
protected:
    std::istringstream sstream;

public:
    StringTextReader(const std::string &data);
};


// IMPLEMENTATION
// --------------


/** \brief Get key from data.
 */
template <typename T>
T TextReader::key() const
{
    if (!isObject()) {
        throw NodeError("Cannot get key without an object type.");
    }

    detail::Extract<T> extract;
    return extract(buffer()[0]);
}


/** \brief Get value from data.
 */
template <typename T>
T TextReader::value() const
{
    detail::Extract<T> extract;
    return extract(buffer()[1]);
}


}   /* json */
