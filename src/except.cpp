//  :copyright: (c) 2016-2017 The Regents of the University of California.
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


Exception::Exception(const std::string &message):
    message(message)
{}


const char * Exception::what() const noexcept
{
    return message.data();
}

}   /* json */
