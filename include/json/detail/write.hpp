//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Convert common types to string.
 */

#pragma once

#include "type.hpp"

#include <lexi/lexi.hpp>

#include <ostream>


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------


/** \brief Write data to stream.
 */
template <typename T>
enable_if_t<!is_array_v<T> && !is_object_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t)
{
    auto string = lexi::jsonify(t);
    stream.write(string.data(), string.size());

    return stream;
}

/** \brief Forward-declare array specialization.
 */
template <typename T>
enable_if_t<is_array_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t);


/** \brief Forward-declare object specialization.
 */
template <typename T>
enable_if_t<is_object_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t);


/** \brief Write quoted key from quoted value.
 */
template <typename T>
enable_if_t<lexi::is_string_v<T>, std::ostream&>
writeKey(std::ostream &stream,
    const T &t)
{
    auto string = lexi::jsonify(t);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write quoted key from unquoted value.
 */
template <typename T>
enable_if_t<!lexi::is_string_v<T>, std::ostream&>
writeKey(std::ostream &stream,
    const T &t)
{
    auto string = "\"" + lexi::jsonify(t) + "\"";
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write value.
 */
template <typename T>
std::ostream & writeValue(std::ostream &stream,
    const T &t)
{
    return write(stream, t);
}

/** \brief Write value.
 */
template <typename T>
std::ostream & writeValue(std::ostream &stream,
    T &&t)
{
    return write(stream, std::forward<T>(t));
}

// CONTAINERS

/** \brief Write array.
 */
template <typename T>
enable_if_t<is_array_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t)
{
    stream.put('[');
    bool first = true;
    for (const auto &item: t) {
        if (!first) {
            stream.put(',');
        } else {
            first = false;
        }
        write(stream, item);
    }
    stream.put(']');

    return stream;
}


/** \brief Write array.
 */
template <typename T>
enable_if_t<is_object_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t)
{
    stream.put('{');
    bool first = true;
    for (const auto &item: t) {
        if (!first) {
            stream.put(',');
        } else {
            first = false;
        }
        writeKey(stream, item.first);
        stream.put(':');
        writeValue(stream, item.second);
    }
    stream.put('}');

    return stream;
}

}   /* detail */
}   /* json */
