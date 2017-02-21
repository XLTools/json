//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \brief Simple string wrapper for value extraction.
 */

#pragma once

#include <string>


namespace json
{
namespace detail
{
// OBJECTS
// -------


/** \brief Simple value wrapper with implicit type conversions to types.
 */
class StringWrapper
{
protected:
    const char *data_ = nullptr;
    size_t length_ = 0;

public:
    // MEMBER TYPES
    typedef char value_type;
    typedef char& reference;
    typedef const char& const_reference;
    typedef char* pointer;
    typedef const char* const_pointer;
    typedef std::ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<pointer> reverse_iterator;
    typedef std::reverse_iterator<const_pointer> const_reverse_iterator;

    static const size_t npos = SIZE_MAX;

    StringWrapper() = default;
    StringWrapper(const StringWrapper&) = default;
    StringWrapper & operator=(const StringWrapper&) = default;
    StringWrapper(StringWrapper&&) = default;
    StringWrapper & operator=(StringWrapper&&) = default;

    StringWrapper(const std::nullptr_t nullp);
    StringWrapper(const std::string &string);
    StringWrapper(const char *string);
    StringWrapper(const char *string,
        const size_t length);
    StringWrapper(const char *begin,
        const char *end);

    // DATA
    const char * data() const;
    size_t size() const;
    size_t length() const;
    bool empty() const;

    // ELEMENT ACCESS
    char & front();
    const char & front() const;
    char & back();
    const char & back() const;

    // ITERATORS
    const char * begin() const;
    const char * end() const;

    // OPERATORS
    bool operator==(const StringWrapper &other) const;
    bool operator!=(const StringWrapper &other) const;
    bool operator==(const std::string &other) const;
    bool operator!=(const std::string &other) const;
    bool operator==(const char *other) const;
    bool operator!=(const char *other) const;

    // CONVERSIONS
    explicit operator bool() const;
    explicit operator std::string() const;
};

}   /* detail */
}   /* json */
