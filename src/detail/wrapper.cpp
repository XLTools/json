//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \brief Simple string wrapper for value extraction.
 */

#include "json/detail/wrapper.hpp"

#include <cstring>


namespace json
{
namespace detail
{
// OBJECTS
// -------


/** \brief Null constructor.
 */
StringWrapper::StringWrapper(const std::nullptr_t nullp):
    data_(nullptr),
    length_(0)
{}


/** \brief Initializer list constructor.
 */
StringWrapper::StringWrapper(const std::string &string):
    data_(string.data()),
    length_(string.size())
{}


/** \brief Initializer list constructor.
 */
StringWrapper::StringWrapper(const char *string):
    data_(string),
    length_(strlen(string))
{}


/** \brief Initializer list constructor.
 */
StringWrapper::StringWrapper(const char *string,
        const size_t length):
    data_(string),
    length_(length)
{}


/** \brief Initializer list constructor.
 */
StringWrapper::StringWrapper(const char *begin,
        const char *end):
    data_(begin),
    length_(end - begin)
{}


/** \brief Get pointer to array.
 */
const char * StringWrapper::data() const
{
    return data_;
}


/** \brief Get length of string.
 */
size_t StringWrapper::size() const
{
    return length_;
}


/** \brief Alias for size().
 */
size_t StringWrapper::length() const
{
    return length_;
}


/** \brief Check if type is empty.
 */
bool StringWrapper::empty() const
{
    return length_ == 0;
}


/** \brief Access front element.
 */
char & StringWrapper::front()
{
    return const_cast<char&>(*data_);
}


/** \brief Access front element.
 */
const char & StringWrapper::front() const
{
    return *data_;
}


/** \brief Access back element.
 */
char & StringWrapper::back()
{
    return const_cast<char&>(*(data_ + length_ - 1));
}


/** \brief Access back element.
 */
const char & StringWrapper::back() const
{
    return *(data_ + length_ - 1);
}


/** \brief Get iterator at beginning of array.
 */
const char * StringWrapper::begin() const
{
    return data();
}


/** \brief Get iterator past end of array.
 */
const char * StringWrapper::end() const
{
    return begin() + size();
}


/** \brief Equality operator.
 */
bool StringWrapper::operator==(const StringWrapper &other) const
{
    if (length() == other.length()) {
        return !strncmp(data(), other.data(), length());
    }

    return false;
}


/** \brief Inequality operator.
 */
bool StringWrapper::operator!=(const StringWrapper &other) const
{
    return !operator==(other);
}


/** \brief Equality operator.
 */
bool StringWrapper::operator==(const std::string &other) const
{
    if (length() == other.length()) {
        return !strncmp(data(), other.data(), length());
    }

    return false;
}


/** \brief Inequality operator.
 */
bool StringWrapper::operator!=(const std::string &other) const
{
    return !operator==(other);
}


/** \brief Equality operator.
 */
bool StringWrapper::operator==(const char *other) const
{
    if (length() == strlen(other)) {
        return !strncmp(data(), other, length());
    }

    return false;
}


/** \brief Inequality operator.
 */
bool StringWrapper::operator!=(const char *other) const
{
    return !operator==(other);
}


/** \brief Convert to bool.
 */
StringWrapper::operator bool() const
{
    return length_ > 0;
}


/** \brief Convert to std::string.
 */
StringWrapper::operator std::string() const
{
    return std::string(data_, length_);
}

}   /* detail */
}   /* json */
