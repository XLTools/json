//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief String utilities.
 */

#pragma once

#include <string>


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------

/** \brief Escape printable, non-Unicode, non-alphanumeric characters.
 */
std::string escape(const std::string &src);

}   /* detail */
}   /* json */
