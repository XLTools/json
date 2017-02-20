//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Public header for JSON TextWriter.
 */

#pragma once

#include "node.hpp"
#include "detail/write.hpp"
#include "detail/type.hpp"

#include <deque>
#include <ostream>


namespace json
{
// OBJECTS
// -------


/** \brief
 */
class TextWriter
{
protected:
    std::ostream *stream = nullptr;
    std::deque<NodeType> node;
    std::deque<size_t> offset;
//    const bool pretty = false;
//    const std::string indent;
//    const std::string space;
//    const std::string newline;

    void checkNode(const NodeType type) const;
    void writeValueDelimiter();
    void writeKeyDelimiter();

public:
    TextWriter() = delete;
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
    bool endObject();
    bool startArray();
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


// IMPLEMENTATION
// --------------


/** \brief Write custom value to array.
 */
template <typename T>
void TextWriter::write(const T &t)
{
    if (!(detail::is_array_v<T> || detail::is_object_v<T>)) {
        checkNode(NodeType::ARRAY);
    }
    writeValueDelimiter();
    detail::write(*stream, t);
}


/** \brief Write custom value to array with perfect forwarding.
 */
template <typename T>
void TextWriter::write(T &&t)
{
    if (!(detail::is_array_v<T> || detail::is_object_v<T>)) {
        checkNode(NodeType::ARRAY);
    }
    writeValueDelimiter();
    detail::write(*stream, std::forward<T>(t));
}


/** \brief Write custom value to object.
 */
template <typename T, typename U>
void TextWriter::write(const T &t, const U &u)
{
    checkNode(NodeType::OBJECT);
    writeValueDelimiter();
    detail::write(*stream, t);
    writeKeyDelimiter();
    detail::write(*stream, u);
}


/** \brief Write custom value to object with perfect forwarding.
 */
template <typename T, typename U>
void TextWriter::write(T &&t, U &&u)
{
    checkNode(NodeType::OBJECT);
    writeValueDelimiter();
    detail::write(*stream, std::forward<T>(t));
    writeKeyDelimiter();
    detail::write(*stream, std::forward<U>(u));
}

}   /* json */
