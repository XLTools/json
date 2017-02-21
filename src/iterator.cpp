//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Iterators for TextReader views.
 */

#include "json/reader.hpp"


namespace json
{
// CONSTANTS
// ---------

static const std::string EMPTY;

// OBJECTS
// -------


/** \brief Initializer list constructor.
 */
ArrayIterator::ArrayIterator(TextReader *reader):
    reader(reader),
    data(EMPTY)
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
    data = ArrayData(reader->buffer_[1]);
    return data;
}


/** \brief Dereference operator.
 */
ArrayData * ArrayIterator::operator->()
{
    data = ArrayData(reader->buffer_[1]);
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

    // exited node
    if (reader->depth() < depth) {
        reader = nullptr;
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
    reader(reader),
    data(EMPTY, EMPTY)
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

    // nullify iterator
    if (reader->depth() >= depth) {
        data.first = ValueWrapper(reader->buffer_[0]);
        data.second = ValueWrapper(reader->buffer_[1]);
    } else {
        reader = nullptr;
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
