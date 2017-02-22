//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief JSON exceptions.
 */

#pragma once

#include <stdexcept>
#include <string>


namespace json
{
// EXCEPTIONS
// ----------


/** \brief Base class for JSON exceptions.
 */
class Exception: public std::exception
{
protected:
    std::string message;

public:
    Exception(const std::string &message);
    virtual const char * what() const noexcept;
};


/** \brief Generic parsing error.
 */
struct ParserError: Exception
{
    using Exception::Exception;
};


/** \brief Generic parsing error.
 */
struct NodeError: Exception
{
    using Exception::Exception;
};

}   /* json */
