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


const std::string & KeyWrapper::data() const
{
    JSON_ASSERT(reader, "KeyWrapper::data() : reader is null.");
    return reader->buffer_[0];
}


KeyWrapper::KeyWrapper(const TextReader *reader):
    reader(reader)
{}


const std::string & ValueWrapper::data() const
{
    JSON_ASSERT(reader, "ValueWrapper::data() : reader is null.");
    return reader->buffer_[1];
}


ValueWrapper::ValueWrapper(const TextReader *reader):
    reader(reader)
{}


bool ValueWrapper::is_null() const
{
    JSON_ASSERT(reader, "ValueWrapper::is_null() : reader is null.");
    return reader->is_null();
}


bool ValueWrapper::is_bool() const
{
    JSON_ASSERT(reader, "ValueWrapper::is_bool() : reader is null.");
    return reader->is_bool();
}


bool ValueWrapper::is_number() const
{
    JSON_ASSERT(reader, "ValueWrapper::is_number() : reader is null.");
    return reader->is_number();
}


bool ValueWrapper::is_string() const
{
    JSON_ASSERT(reader, "ValueWrapper::is_string() : reader is null.");
    return reader->is_string();
}


ValueType ValueWrapper::type() const
{
    JSON_ASSERT(reader, "ValueWrapper::type() : reader is null.");
    return reader->type();
}


ArrayIterator::ArrayIterator(TextReader *reader):
    reader(reader)
{
    if (reader && reader->is_array() && !reader->is_end_node()) {
        depth = reader->depth();
        if (reader->is_start_node()) {
            operator++();
        }
    } else if (reader) {
        throw ParserError("Expected array for iterator, got non-array node.");
    }
}


bool ArrayIterator::operator==(const ArrayIterator &other) const
{
    return reader == other.reader;
}


bool ArrayIterator::operator!=(const ArrayIterator &other) const
{
    return !operator==(other);
}


ArrayData & ArrayIterator::operator*()
{
    return data;
}


ArrayData * ArrayIterator::operator->()
{
    return &data;
}


ArrayIterator & ArrayIterator::operator++()
{
    if (!reader || !reader->is_valid()) {
        reader = nullptr;
        return *this;
    }

    // seek next node
    do {
        reader->read();
        if (reader->is_end_node()) {
            continue;
        } else if (reader->depth() == depth && !reader->is_start_node()) {
            break;
        } else if (reader->depth() == depth+1 && reader->is_start_node()) {
            break;
        }
    } while (reader->is_valid() && reader->depth() >= depth);

    // get value or nullify iterator
    if (reader->depth() >= depth && !reader->is_end_node()) {
        data = ValueWrapper(reader);
    } else {
        reader = nullptr;
        data = ValueWrapper(nullptr);
    }

    return *this;
}


ArrayIterator ArrayIterator::operator++(int)
{
    ArrayIterator copy(*this);
    ++(*this);

    return copy;
}


void ArrayIterator::swap(ArrayIterator &other)
{
    auto tmp = other.reader;
    other.reader = reader;
    reader = tmp;
}


ObjectIterator::ObjectIterator(TextReader *reader):
    reader(reader)
{
    if (reader && reader->is_object() && !reader->is_end_node()) {
        depth = reader->depth();
        if (reader->is_start_node()) {
            operator++();
        }
    } else if (reader) {
        throw ParserError("Expected object for iterator, got non-object node.");
    }
}


bool ObjectIterator::operator==(const ObjectIterator &other) const
{
    return reader == other.reader;
}


bool ObjectIterator::operator!=(const ObjectIterator &other) const
{
    return !operator==(other);
}


ObjectData & ObjectIterator::operator*()
{
    return data;
}


ObjectData * ObjectIterator::operator->()
{
    return &data;
}


ObjectIterator & ObjectIterator::operator++()
{
    if (!reader || !reader->is_valid()) {
        reader = nullptr;
        return *this;
    }

    // seek next node
    do {
        reader->read();
        if (reader->is_end_node()) {
            continue;
        } else if (reader->depth() == depth && !reader->is_start_node()) {
            break;
        } else if (reader->depth() == depth+1 && reader->is_start_node()) {
            break;
        }
    } while (reader->is_valid() && reader->depth() >= depth);

    // get value or nullify iterator
    if (reader->depth() >= depth) {
        data = std::make_pair(KeyWrapper(reader), ValueWrapper(reader));
    } else {
        reader = nullptr;
        data = std::make_pair(KeyWrapper(nullptr), ValueWrapper(nullptr));
    }

    return *this;
}


ObjectIterator ObjectIterator::operator++(int)
{
    ObjectIterator copy(*this);
    ++(*this);

    return copy;
}


void ObjectIterator::swap(ObjectIterator &other)
{
    auto tmp = other.reader;
    other.reader = reader;
    reader = tmp;
}

}   /* json */
