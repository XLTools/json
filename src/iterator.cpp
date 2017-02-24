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
    JSON_ASSERT(reader, "KeyWrapper::data() : reader is null.");
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
    JSON_ASSERT(reader, "ValueWrapper::data() : reader is null.");
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
    JSON_ASSERT(reader, "ValueWrapper::isNull() : reader is null.");
    return reader->isNull();
}


/** \brief Check if current value is bool.
 */
bool ValueWrapper::isBool() const
{
    JSON_ASSERT(reader, "ValueWrapper::isBool() : reader is null.");
    return reader->isBool();
}


/** \brief Check if current value is a number.
 */
bool ValueWrapper::isNumber() const
{
    JSON_ASSERT(reader, "ValueWrapper::isNumber() : reader is null.");
    return reader->isNumber();
}


/** \brief Check if current value is string.
 */
bool ValueWrapper::isString() const
{
    JSON_ASSERT(reader, "ValueWrapper::isString() : reader is null.");
    return reader->isString();
}


/** \brief Get underlying value type.
 */
ValueType ValueWrapper::type() const
{
    JSON_ASSERT(reader, "ValueWrapper::type() : reader is null.");
    return reader->type();
}


/** \brief Initializer list constructor.
 */
ArrayIterator::ArrayIterator(TextReader *reader):
    reader(reader)
{
    if (reader && reader->isArray() && !reader->isEndNode()) {
        depth = reader->depth();
        if (reader->isStartNode()) {
            operator++();
        }
    } else if (reader) {
        throw ParserError("Expected array for iterator, got non-array node.");
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
        if (reader->isEndNode()) {
            continue;
        } else if (reader->depth() == depth && !reader->isStartNode()) {
            break;
        } else if (reader->depth() == depth+1 && reader->isStartNode()) {
            break;
        }
    } while (reader->isValid() && reader->depth() >= depth);

    // get value or nullify iterator
    if (reader->depth() >= depth && !reader->isEndNode()) {
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
    printf("Constructing new iterator\n");
    if (reader && reader->isObject() && !reader->isEndNode()) {
        depth = reader->depth();
        if (reader->isStartNode()) {
            operator++();
        }
    } else if (reader) {
        throw ParserError("Expected object for iterator, got non-object node.");
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
        if (reader->isEndNode()) {
            continue;
        } else if (reader->depth() == depth && !reader->isStartNode()) {
            break;
        } else if (reader->depth() == depth+1 && reader->isStartNode()) {
            break;
        }
    } while (reader->isValid() && reader->depth() >= depth);

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
