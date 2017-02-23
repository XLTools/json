//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Public header for JSON TextWriter.
 */

#pragma once

#include "except.hpp"
#include "node.hpp"
#include "detail/write.hpp"
#include "detail/type.hpp"

#include <deque>
#include <fstream>
#include <ostream>
#include <sstream>


namespace json
{
// MACROS
// ------

/** \brief Check the current element is an array element.
 */
#define JSON_CHECK_ARRAY()                                              \
    if (!isArray()) {                                                   \
        throw NodeError("TextWriter::isArray() -> false.");             \
    }

/** \brief Check the current element is an array element, or root element.
 */
#define JSON_CHECK_ROOT_OR_ARRAY()                                      \
    if (!node.empty() && !isArray()) {                                  \
        throw NodeError("TextWriter::isArray() -> false.");             \
    }

/** \brief Check the current element is an object element.
 */
#define JSON_CHECK_OBJECT()                                             \
    if (!isObject()) {                                                  \
        throw NodeError("TextWriter::isObject() -> false.");            \
    }

// OBJECTS
// -------


/** \brief Base text writer for JSON serialization.
 */
class TextWriter
{
protected:
    std::ostream *stream = nullptr;
    std::deque<NodeType> node;
    std::deque<size_t> offset;

    TextWriter() = default;
    void open(std::ostream &stream);

    void checkNode(const NodeType type) const;
    void writeValueDelimiter();
    void writeKeyDelimiter();

public:
    TextWriter(const TextWriter&) = delete;
    TextWriter & operator=(const TextWriter&) = delete;
    TextWriter(TextWriter&&) = default;
    TextWriter & operator=(TextWriter&&) = default;

    TextWriter(std::ostream &stream);

    // PROPERTIES
    size_t depth() const;
    bool isBad() const;
    bool isValid() const;
    bool isArray() const;
    bool isObject() const;

    // WRITERS -- NODES
    bool startObject();
    bool startObject(const std::string &key);
    bool endObject();
    bool startArray();
    bool startArray(const std::string &key);
    bool endArray();

    // WRITERS -- ARRAYS
    template <typename T>
    void write(const T &t);
    template <typename T>
    void write(T &&t);

    // WRITERS -- OBJECTS
    template <typename T, typename U>
    void write(const T &t, const U &u);
    template <typename T, typename U>
    void write(T &&t, U &&u);
};


/** \brief Text writer for file-backed JSON stores.
 */
class FileTextWriter: public TextWriter
{
protected:
    std::ofstream fstream;

public:
    FileTextWriter(const std::string &path);
};


/** \brief Text writer for string-based JSON stores.
 */
class StringTextWriter: public TextWriter
{
protected:
    std::ostringstream sstream;

public:
    StringTextWriter();

    std::string str() const;
};


// IMPLEMENTATION
// --------------


/** \brief Write custom value to array.
 */
template <typename T>
void TextWriter::write(const T &t)
{
    JSON_CHECK_ROOT_OR_ARRAY();

    writeValueDelimiter();
    detail::write(*stream, t);
}


/** \brief Write custom value to array with perfect forwarding.
 */
template <typename T>
void TextWriter::write(T &&t)
{
    JSON_CHECK_ROOT_OR_ARRAY();

    writeValueDelimiter();
    detail::write(*stream, std::forward<T>(t));
}


/** \brief Write custom value to object.
 */
template <typename T, typename U>
void TextWriter::write(const T &t, const U &u)
{
    JSON_CHECK_OBJECT();

    writeValueDelimiter();
    detail::writeKey(*stream, t);
    writeKeyDelimiter();
    detail::writeValue(*stream, u);
}


/** \brief Write custom value to object with perfect forwarding.
 */
template <typename T, typename U>
void TextWriter::write(T &&t, U &&u)
{
    JSON_CHECK_OBJECT();

    writeValueDelimiter();
    detail::writeKey(*stream, std::forward<T>(t));
    writeKeyDelimiter();
    detail::writeValue(*stream, std::forward<U>(u));
}

}   /* json */
