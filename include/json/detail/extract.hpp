//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Extract data from encoded strings.
 */

#pragma once

#include "type.hpp"
#include "wrapper.hpp"


namespace json
{
namespace detail
{
// OBJECTS
// -------

// TODO: need to replace with the wrapper

/** \brief Extract basic value from string.
 */
template <typename T>
struct Extract
{
    /** \brief Overload string.
     */
    template <typename U = T>
    enable_if_t<is_string_v<U>, const U&>
    operator()(const std::string &string)
    {
        return string;
    }

    /** \brief Overload char.
     */
    template <typename U = T>
    enable_if_t<is_char_v<U>, U>
    operator()(const std::string &string)
    {
        return string.empty() ? '\0' : string.front();
    }

    /** \brief Overload boolean.
     */
    template <typename U = T>
    enable_if_t<is_bool_v<U>, U>
    operator()(const std::string &string)
    {
        return string == "true";
    }

    /** \brief Overload integer.
     */
    template <typename U = T>
    enable_if_t<is_integer_v<U>, U>
    operator()(const std::string &string)
    {
        // TODO: need to optimize
        return std::stol(std::string(string));
    }

    /** \brief Overload float.
     */
    template <typename U = T>
    enable_if_t<is_float_v<U>, U>
    operator()(const std::string &string)
    {
        if (string == "NaN") {
            return std::numeric_limits<T>::quiet_NaN();
        } else if (string == "Infinity") {
            return std::numeric_limits<T>::infinity();
        } else if (string == "-Infinity") {
            return -std::numeric_limits<T>::infinity();
        }
        // TODO: need to optimize
        return std::stod(std::string(string));
    }
};

}   /* detail */
}   /* json */
