//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Public header for JSON TextReader.
 */

#pragma once

#include "except.hpp"
#include "view.hpp"

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
    void start_object();
    void end_object();
    void start_array();
    void end_array();

    // ELEMENTS
    void increment();

    // PARSERS -- SPECIFIC
    void parse_key();
    void parse_value();
    void parse_pair();
    void parse_array();
    void parse_object();

    // PARSERS -- GENERIC
    void parse_start();
    void parse_node();

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
    bool is_eof() const;
    bool is_bad() const;
    bool is_valid() const;
    bool is_null() const;
    bool is_bool() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    bool is_start_node() const;
    bool is_end_node() const;
    bool has_key() const;
    bool has_value() const;
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


template <typename T>
T TextReader::key() const
{
    return T(KeyWrapper(this));
}


template <typename T>
T TextReader::value() const
{
    return T(ValueWrapper(this));
}

}   /* json */
