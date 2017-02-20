//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief JSON exceptions.
 */

#include "json/except.hpp"


namespace json
{
// EXCEPTIONS
// ----------


/** \brief Initializer list constructor.
 */
Exception::Exception(const std::string &message):
    message(message)
{}


/** \brief Print error message.
 */
const char * Exception::what() const noexcept
{
    return message.data();
}

}   /* json */
