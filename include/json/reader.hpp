//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Public header for JSON TextReader.
 */

#pragma once

#include "except.hpp"
#include "view.hpp"
#include "detail/extract.hpp"

#include <array>
#include <deque>
#include <fstream>
#include <istream>
#include <sstream>


namespace json
{
namespace detail
{
// DETAIL
// ------


/** \brief Private base class for JSON parsing.
 */
class TextReaderImpl
{
protected:
    std::istream *stream_ = nullptr;
    std::deque<NodeType> node_;
    std::deque<size_t> offset_;
    std::array<std::string, 2> buffer_;
    ValueType type_;

    // NODES
    void startObject();
    void endObject();
    void startArray();
    void endArray();

    // ELEMENTS
    void increment();

    // PARSERS -- SPECIFIC
    void parseKey();
    void parseValue();
    void parsePair();
    void parseArray();
    void parseObject();

    // PARSERS -- GENERIC
    void parseStart();
    void parseNode();

    TextReaderImpl() = default;
    void open(std::istream &stream);

public:
    TextReaderImpl(std::istream &stream);
};


}   /* detail */

// OBJECTS
// -------


/** \brief Base text reader for JSON parsing.
 */
class TextReader: protected detail::TextReaderImpl
{
protected:
    TextReader() = default;

    friend class KeyWrapper;
    friend class ValueWrapper;

public:
    TextReader(const TextReader&) = delete;
    TextReader & operator=(const TextReader&) = delete;
    TextReader(TextReader&&) = default;
    TextReader & operator=(TextReader&&) = default;

    TextReader(std::istream &stream);

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
    bool isStartNode() const;
    bool isEndNode() const;
    bool hasKey() const;
    bool hasValue() const;
    ValueType type() const;

    // PROCESSING
    bool read();
    bool seek(const size_t depth);
    bool seek(const std::string &key);
    bool seek(const std::string &key,
        const size_t depth);

    // EXTRACT
    template <typename T = std::string>
    T key() const;

    template <typename T = std::string>
    T value() const;

    // VIEWS
    ArrayView array();
    ObjectView object();
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
    return T(KeyWrapper(this));
}


/** \brief Get value from data.
 */
template <typename T>
T TextReader::value() const
{
    return T(ValueWrapper(this));
}

}   /* json */
