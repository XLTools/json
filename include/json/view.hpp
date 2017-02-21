//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Object and array views for the TextReader.
 */

#pragma once

#include "iterator.hpp"

#include <string>
#include <utility>


namespace json
{
// OBJECTS
// -------


/** \brief TextReader view specialized for parsing arrays.
 */
class ArrayView
{
protected:
    TextReader *reader = nullptr;

public:
    typedef ArrayIterator iterator;

    ArrayView() = delete;
    ArrayView(const ArrayView&) = default;
    ArrayView & operator=(const ArrayView&) = default;
    ArrayView(ArrayView&&) = default;
    ArrayView & operator=(ArrayView&&) = default;

    ArrayView(TextReader &reader);

    iterator begin();
    iterator end();
};


/** \brief TextReader view specialized for parsing objects.
 */
class ObjectView
{
protected:
    TextReader *reader = nullptr;

public:
    typedef ObjectIterator iterator;

    ObjectView() = delete;
    ObjectView(const ObjectView&) = default;
    ObjectView & operator=(const ObjectView&) = default;
    ObjectView(ObjectView&&) = default;
    ObjectView & operator=(ObjectView&&) = default;

    ObjectView(TextReader &reader);

    iterator begin();
    iterator end();
};

}   /* json */
