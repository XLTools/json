//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Iterators for TextReader views.
 */

#pragma once

#include "node.hpp"

#include <lexi/lexi.hpp>

#include <iterator>


namespace json
{
// FORWARD
// -------

class TextReader;
class KeyWrapper;
class ValueWrapper;

// TYPES
// -----

typedef ValueWrapper ArrayData;
typedef std::pair<KeyWrapper, ValueWrapper> ObjectData;

// OBJECTS
// -------


/** \brief Simple wrapper for JSON keys.
 */
class KeyWrapper
{
protected:
    const TextReader *reader = nullptr;

    const std::string & data() const;

public:
    KeyWrapper() = default;
    KeyWrapper(const KeyWrapper&) = default;
    KeyWrapper & operator=(const KeyWrapper&) = default;
    KeyWrapper(KeyWrapper&&) = default;
    KeyWrapper & operator=(KeyWrapper&&) = default;

    KeyWrapper(const TextReader *reader);

    template <typename T>
    explicit operator T() const;
};


/** \brief Simple wrapper for JSON values.
 */
class ValueWrapper
{
protected:
    const TextReader *reader = nullptr;

    const std::string & data() const;

public:
    ValueWrapper() = default;
    ValueWrapper(const ValueWrapper&) = default;
    ValueWrapper & operator=(const ValueWrapper&) = default;
    ValueWrapper(ValueWrapper&&) = default;
    ValueWrapper & operator=(ValueWrapper&&) = default;

    ValueWrapper(const TextReader *reader);

    bool isNull() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    ValueType type() const;

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
KeyWrapper::operator T() const
{
    return lexi::lexi<T>(data());
}


/** \brief Custom type conversion operation.
 */
template <typename T>
ValueWrapper::operator T() const
{
    return lexi::lexi<T>(data());
}

}   /* json */
