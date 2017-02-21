//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Convert common types to string.
 */

#pragma once

#include "type.hpp"

#include <ostream>
#include <string>


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------

/** \brief Write null.
 */
std::ostream & write(std::ostream &stream,
    const std::nullptr_t nullp);

/** \brief Write bool.
 */
std::ostream & write(std::ostream &stream,
    const bool boolean);

/** \brief Write C-string.
 */
std::ostream & write(std::ostream &stream,
    const char *string);

/** \brief Write string.
 */
std::ostream & write(std::ostream &stream,
    const std::string &string);

/** \brief Write character.
 */
std::ostream & write(std::ostream &stream,
    const char character);

/** \brief Write unsigned character.
 */
std::ostream & write(std::ostream &stream,
    const unsigned char character);

/** \brief Write float.
 */
std::ostream & write(std::ostream &stream,
    const float number);

/** \brief Write double.
 */
std::ostream & write(std::ostream &stream,
    const double number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int16_t number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint16_t number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int32_t number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint32_t number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int64_t number);

/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint64_t number);

/** \brief Write null key.
 */
std::ostream & writeKey(std::ostream &stream,
    const std::nullptr_t nullp);

/** \brief Write bool.
 */
std::ostream & writeKey(std::ostream &stream,
    const bool boolean);

/** \brief Write C-string.
 */
std::ostream & writeKey(std::ostream &stream,
    const char *string);

/** \brief Write string.
 */
std::ostream & writeKey(std::ostream &stream,
    const std::string &string);

/** \brief Write character.
 */
std::ostream & writeKey(std::ostream &stream,
    const char character);

/** \brief Write unsigned character.
 */
std::ostream & writeKey(std::ostream &stream,
    const unsigned char character);

/** \brief Write float.
 */
std::ostream & writeKey(std::ostream &stream,
    const float number);

/** \brief Write double.
 */
std::ostream & writeKey(std::ostream &stream,
    const double number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int16_t number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint16_t number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int32_t number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint32_t number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int64_t number);

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint64_t number);

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
