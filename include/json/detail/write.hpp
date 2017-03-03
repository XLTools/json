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


template <typename T>
enable_if_t<!is_array_v<T> && !is_object_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t)
{
    auto string = lexi::jsonify(t);
    stream.write(string.data(), string.size());

    return stream;
}

template <typename T>
enable_if_t<is_array_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t);


template <typename T>
enable_if_t<is_object_v<T>, std::ostream&>
write(std::ostream &stream,
    const T &t);


template <typename T>
enable_if_t<lexi::is_string_v<T>, std::ostream&>
write_key(std::ostream &stream,
    const T &t)
{
    auto string = lexi::jsonify(t);
    stream.write(string.data(), string.size());

    return stream;
}


template <typename T>
enable_if_t<!lexi::is_string_v<T>, std::ostream&>
write_key(std::ostream &stream,
    const T &t)
{
    auto string = "\"" + lexi::jsonify(t) + "\"";
    stream.write(string.data(), string.size());

    return stream;
}


template <typename T>
std::ostream & write_value(std::ostream &stream,
    const T &t)
{
    return write(stream, t);
}

template <typename T>
std::ostream & write_value(std::ostream &stream,
    T &&t)
{
    return write(stream, std::forward<T>(t));
}

// CONTAINERS

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
        write_key(stream, item.first);
        stream.put(':');
        write_value(stream, item.second);
    }
    stream.put('}');

    return stream;
}

}   /* detail */
}   /* json */
