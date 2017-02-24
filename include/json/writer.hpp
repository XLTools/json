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
    if (!isArray() || intermediate) {                                   \
        throw NodeError("TextWriter::isArray() -> false.");             \
    }

/** \brief Check the current element takes a key.
 *
 *  Must be an object and not have a key value.
 *  Use for node-start and end from object elements, since it must
 *  not be in an intermediate state.
 */
#define JSON_ACCEPTS_KEY()                                              \
    if (!isObject() || intermediate) {                                  \
        throw NodeError("TextWriter does not accept key.");             \
    }


/** \brief Check the current element takes a value.
 *
 *  Can be an array, or be an object and have an intermediate value,
 *  or have no-root element.
 */
#define JSON_ACCEPTS_VALUE()                                            \
    if (!((isObject() && intermediate) || isArray() || node.empty())) { \
        throw NodeError("TextWriter does not accept value.");           \
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
    bool intermediate = false;

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

    template <typename T>
    void writeKey(const T &t);
    template <typename T>
    void writeKey(T &&t);
    template <typename T>
    void writeValue(const T &t);
    template <typename T>
    void writeValue(T &&t);
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
    if (isObject()) {
        if (intermediate) {
            writeKey(t);
        } else {
            writeValue(t);
        }
    } else {
        writeValueDelimiter();
        detail::write(*stream, t);
    }
}


/** \brief Write custom value to array with perfect forwarding.
 */
template <typename T>
void TextWriter::write(T &&t)
{
    if (isObject()) {
        if (intermediate) {
            writeKey(std::forward<T>(t));
        } else {
            writeValue(std::forward<T>(t));
        }
    } else {
        writeValueDelimiter();
        detail::write(*stream, std::forward<T>(t));
    }
}


/** \brief Write custom value to object.
 */
template <typename T, typename U>
void TextWriter::write(const T &t, const U &u)
{
    JSON_ACCEPTS_KEY();

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
    JSON_ACCEPTS_KEY();

    writeValueDelimiter();
    detail::writeKey(*stream, std::forward<T>(t));
    writeKeyDelimiter();
    detail::writeValue(*stream, std::forward<U>(u));
}


/** \brief Write key to object.
 */
template <typename T>
void TextWriter::writeKey(const T &t)
{
    JSON_ACCEPTS_KEY();

    writeValueDelimiter();
    detail::writeKey(*stream, t);
    writeKeyDelimiter();
    intermediate = true;
}


/** \brief Write key to object with perfect forwarding.
 */
template <typename T>
void TextWriter::writeKey(T &&t)
{
    JSON_ACCEPTS_KEY();

    writeValueDelimiter();
    detail::writeKey(*stream, std::forward<T>(t));
    writeKeyDelimiter();
    intermediate = true;
}


/** \brief Write value to object.
 */
template <typename T>
void TextWriter::writeValue(const T &t)
{
    JSON_ACCEPTS_VALUE();

    detail::writeValue(*stream, t);
    intermediate = false;
}


/** \brief Write value to object with perfect forwarding.
 */
template <typename T>
void TextWriter::writeValue(T &&t)
{
    JSON_ACCEPTS_VALUE();

    detail::writeValue(*stream, std::forward<T>(t));
    intermediate = false;
}



}   /* json */
