//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Iterators for TextReader views.
 */

#pragma once

#include "detail/extract.hpp"
#include "detail/wrapper.hpp"

#include <iterator>


namespace json
{
// FORWARD
// -------

class TextReader;
struct ValueWrapper;

// TYPES
// -----

typedef ValueWrapper ArrayData;
typedef std::pair<ValueWrapper, ValueWrapper> ObjectData;

// OBJECTS
// -------


/** \brief Simple string-wrapper with implicit type conversions.
 */
struct ValueWrapper: detail::StringWrapper
{
    using detail::StringWrapper::StringWrapper;

    template <typename T>
    explicit operator T() const;
};


/** \brief Iterator over array data.
 */
class ArrayIterator: public std::iterator<std::input_iterator_tag, ArrayData>
{
protected:
    TextReader *reader = nullptr;
    size_t depth;
    ArrayData data;

public:
    ArrayIterator(const ArrayIterator&) = default;
    ArrayIterator & operator=(const ArrayIterator&) = default;
    ArrayIterator(ArrayIterator&&) = default;
    ArrayIterator & operator=(ArrayIterator&&) = default;

    ArrayIterator(TextReader *reader = nullptr);

    bool operator==(const ArrayIterator &other) const;
    bool operator!=(const ArrayIterator &other) const;
    ArrayData & operator*();
    ArrayData * operator->();
    ArrayIterator & operator++();
    ArrayIterator operator++(int);
    void swap(ArrayIterator &other);
};


/** \brief Iterator over key-value pairs.
 */
class ObjectIterator: public std::iterator<std::input_iterator_tag, ObjectData>
{
protected:
    TextReader *reader = nullptr;
    size_t depth;
    ObjectData data;

public:
    ObjectIterator(const ObjectIterator&) = default;
    ObjectIterator & operator=(const ObjectIterator&) = default;
    ObjectIterator(ObjectIterator&&) = default;
    ObjectIterator & operator=(ObjectIterator&&) = default;

    ObjectIterator(TextReader *reader = nullptr);

    bool operator==(const ObjectIterator &other) const;
    bool operator!=(const ObjectIterator &other) const;
    ObjectData & operator*();
    ObjectData * operator->();
    ObjectIterator & operator++();
    ObjectIterator operator++(int);
    void swap(ObjectIterator &other);
};


// IMPLEMENTATION
// --------------


/** \brief Custom type conversion operation.
 */
template <typename T>
ValueWrapper::operator T() const
{
    detail::Extract<T> extract;
    return extract(data_);
}

}   /* json */
