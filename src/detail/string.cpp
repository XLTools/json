//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief String utilities.
 */

#include "json/detail/string.hpp"


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------

/** \brief Escape printable, non-Unicode, non-alphanumeric characters.
 *
 *  Escapes all ASCII non-alphanumeric characters, assuming
 *  UTF-8 source encoding.
 */
std::string escape(const std::string &src)
{
    std::string dst;
    dst.reserve(src.size() * 2 + 1);

    for (const char c: src) {
        if ((c >= 0 && c <= 47) ||      // Null - /
            (c >= 58 && c <= 64) ||     // : - @
            (c >= 91 && c <= 96) ||     // [ - `]
            (c >= 123 && c <= 126)) {   // ( - ~
            dst.push_back('\\');
        }
        dst.push_back(c);
    }

    dst.shrink_to_fit();
    return dst;
}

}   /* detail */
}   /* json */
