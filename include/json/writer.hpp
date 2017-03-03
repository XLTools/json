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
    if (!is_array() || intermediate) {                                   \
        throw NodeError("TextWriter::is_array() -> false.");             \
    }

/** \brief Check the current element takes a key.
 *
 *  Must be an object and not have a key value.
 *  Use for node-start and end from object elements, since it must
 *  not be in an intermediate state.
 */
#define JSON_ACCEPTS_KEY()                                              \
    if (!is_object() || intermediate) {                                  \
        throw NodeError("TextWriter does not accept key.");             \
    }


/** \brief Check the current element takes a value.
 *
 *  Can be an array, or be an object and have an intermediate value,
 *  or have no-root element.
 */
#define JSON_ACCEPTS_VALUE()                                            \
    if (!((is_object() && intermediate) || is_array() || node.empty())) { \
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

    void check_node(const NodeType type) const;
    void write_value_delimiter();
    void write_key_delimiter();

public:
    TextWriter(const TextWriter&) = delete;
    TextWriter & operator=(const TextWriter&) = delete;
    TextWriter(TextWriter&&) = default;
    TextWriter & operator=(TextWriter&&) = default;

    TextWriter(std::ostream &stream);

    // PROPERTIES
    size_t depth() const;
    bool is_bad() const;
    bool is_valid() const;
    bool is_array() const;
    bool is_object() const;

    // WRITERS -- NODES
    bool start_object();
    bool start_object(const std::string &key);
    bool end_object();
    bool start_array();
    bool start_array(const std::string &key);
    bool end_array();

    // WRITERS -- ARRAYS
    template <typename T, enable_if_t<!is_container_v<T>, T>* = nullptr>
    void write(const T &t);
    template <typename T, enable_if_t<!is_container_v<T>, T>* = nullptr>
    void write(T &&t);
    template <typename T, enable_if_t<is_container_v<T>, T>* = nullptr>
    void write(const T &t);
    template <typename T, enable_if_t<is_container_v<T>, T>* = nullptr>
    void write(T &&t);

    // WRITERS -- OBJECTS
    template <typename T, typename U>
    void write(const T &t, const U &u);
    template <typename T, typename U>
    void write(T &&t, U &&u);

    template <typename T>
    void write_key(const T &t);
    template <typename T>
    void write_key(T &&t);
    template <typename T>
    void write_value(const T &t);
    template <typename T>
    void write_value(T &&t);
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


template <typename T, enable_if_t<!is_container_v<T>, T>*>
void TextWriter::write(const T &t)
{
    if (is_object()) {
        if (intermediate) {
            write_value(t);
        } else {
            write_key(t);
        }
    } else {
        write_value_delimiter();
        detail::write(*stream, t);
    }
}


template <typename T, enable_if_t<!is_container_v<T>, T>*>
void TextWriter::write(T &&t)
{
    if (is_object()) {
        if (intermediate) {
            write_value(std::forward<T>(t));
        } else {
            write_key(std::forward<T>(t));
        }
    } else {
        write_value_delimiter();
        detail::write(*stream, std::forward<T>(t));
    }
}


template <typename T, enable_if_t<is_container_v<T>, T>*>
void TextWriter::write(const T &t)
{
    JSON_ACCEPTS_VALUE();
    write_value_delimiter();
    detail::write(*stream, t);
}


template <typename T, enable_if_t<is_container_v<T>, T>*>
void TextWriter::write(T &&t)
{
    JSON_ACCEPTS_VALUE();
    write_value_delimiter();
    detail::write(*stream, std::forward<T>(t));
}


template <typename T, typename U>
void TextWriter::write(const T &t, const U &u)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    detail::write_key(*stream, t);
    write_key_delimiter();
    detail::write_value(*stream, u);
}


template <typename T, typename U>
void TextWriter::write(T &&t, U &&u)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    detail::write_key(*stream, std::forward<T>(t));
    write_key_delimiter();
    detail::write_value(*stream, std::forward<U>(u));
}


template <typename T>
void TextWriter::write_key(const T &t)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    detail::write_key(*stream, t);
    write_key_delimiter();
    intermediate = true;
}


template <typename T>
void TextWriter::write_key(T &&t)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    detail::write_key(*stream, std::forward<T>(t));
    write_key_delimiter();
    intermediate = true;
}


template <typename T>
void TextWriter::write_value(const T &t)
{
    JSON_ACCEPTS_VALUE();

    detail::write_value(*stream, t);
    intermediate = false;
}


template <typename T>
void TextWriter::write_value(T &&t)
{
    JSON_ACCEPTS_VALUE();

    detail::write_value(*stream, std::forward<T>(t));
    intermediate = false;
}

}   /* json */
