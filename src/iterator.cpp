//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Iterators for TextReader views.
 */

#include "json/reader.hpp"


namespace json
{
// OBJECTS
// -------


/** \brief Access underlying data.
 */
const std::string & KeyWrapper::data() const
{
    return reader->buffer_[0];
}


/** \brief Initializer list constructor.
 */
KeyWrapper::KeyWrapper(const TextReader *reader):
    reader(reader)
{}


/** \brief Access underlying data.
 */
const std::string & ValueWrapper::data() const
{
    return reader->buffer_[1];
}


/** \brief Initializer list constructor.
 */
ValueWrapper::ValueWrapper(const TextReader *reader):
    reader(reader)
{}


/** \brief Check if current value is null.
 */
bool ValueWrapper::isNull() const
{
    return reader->isNull();
}


/** \brief Check if current value is bool.
 */
bool ValueWrapper::isBool() const
{
    return reader->isBool();
}


/** \brief Check if current value is a number.
 */
bool ValueWrapper::isNumber() const
{
    return reader->isNumber();
}


/** \brief Check if current value is string.
 */
bool ValueWrapper::isString() const
{
    return reader->isString();
}


/** \brief Get underlying value type.
 */
ValueType ValueWrapper::type() const
{
    return reader->type();
}


/** \brief Initializer list constructor.
 */
ArrayIterator::ArrayIterator(TextReader *reader):
    reader(reader)
{
    if (reader) {
        depth = reader->depth();
        if (reader->isStartNode()) {
            operator++();
        }
    }
}


/** \brief Equality operator.
 */
bool ArrayIterator::operator==(const ArrayIterator &other) const
{
    return reader == other.reader;
}


/** \brief Inequality operator.
 */
bool ArrayIterator::operator!=(const ArrayIterator &other) const
{
    return !operator==(other);
}


/** \brief Dereference operator.
 */
ArrayData & ArrayIterator::operator*()
{
    return data;
}


/** \brief Dereference operator.
 */
ArrayData * ArrayIterator::operator->()
{
    return &data;
}


/** \brief Pre-increment iterator.
 */
ArrayIterator & ArrayIterator::operator++()
{
    if (!reader || !reader->isValid()) {
        reader = nullptr;
        return *this;
    }

    // seek next node
    do {
        reader->read();
        if (reader->depth() == depth && !reader->isStartNode()) {
            break;
        } else if (reader->depth() == depth+1 && reader->isStartNode()) {
            break;
        }
    } while (reader->isValid() && reader->depth() > depth);

    // get value or nullify iterator
    if (reader->depth() >= depth) {
        data = ValueWrapper(reader);
    } else {
        reader = nullptr;
        data = ValueWrapper(nullptr);
    }

    return *this;
}


/** \brief Post-increment iterator.
 */
ArrayIterator ArrayIterator::operator++(int)
{
    ArrayIterator copy(*this);
    ++(*this);

    return copy;
}


/** \brief Swap iterators.
 */
void ArrayIterator::swap(ArrayIterator &other)
{
    auto tmp = other.reader;
    other.reader = reader;
    reader = tmp;
}


/** \brief Initializer list constructor.
 */
ObjectIterator::ObjectIterator(TextReader *reader):
    reader(reader)
{
    if (reader) {
        depth = reader->depth();
        if (!reader->hasValue()) {
            operator++();
        }
    }
}


/** \brief Equality operator.
 */
bool ObjectIterator::operator==(const ObjectIterator &other) const
{
    return reader == other.reader;
}


/** \brief Inequality operator.
 */
bool ObjectIterator::operator!=(const ObjectIterator &other) const
{
    return !operator==(other);
}


/** \brief Dereference operator.
 */
ObjectData & ObjectIterator::operator*()
{
    return data;
}


/** \brief Dereference operator.
 */
ObjectData * ObjectIterator::operator->()
{
    return &data;
}


/** \brief Pre-increment iterator.
 */
ObjectIterator & ObjectIterator::operator++()
{
    if (!reader || !reader->isValid()) {
        reader = nullptr;
        return *this;
    }

    // seek next node
    do {
        reader->read();
        if (reader->depth() == depth && !reader->isStartNode()) {
            break;
        } else if (reader->depth() == depth+1 && reader->isStartNode()) {
            break;
        }
    } while (reader->isValid() && reader->depth() > depth);

    // get value or nullify iterator
    if (reader->depth() >= depth) {
        data = std::make_pair(KeyWrapper(reader), ValueWrapper(reader));
    } else {
        reader = nullptr;
        data = std::make_pair(KeyWrapper(nullptr), ValueWrapper(nullptr));
    }

    return *this;
}


/** \brief Post-increment iterator.
 */
ObjectIterator ObjectIterator::operator++(int)
{
    ObjectIterator copy(*this);
    ++(*this);

    return copy;
}


/** \brief Swap iterators.
 */
void ObjectIterator::swap(ObjectIterator &other)
{
    auto tmp = other.reader;
    other.reader = reader;
    reader = tmp;
}

}   /* json */
