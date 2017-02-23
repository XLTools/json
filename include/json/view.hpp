//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Object and array views for the TextReader.
 */

#pragma once

#include "iterator.hpp"
#include "detail/type.hpp"

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

    template<
        typename T,
        typename = enable_if_t<is_array_v<T>, T>
    >
    explicit operator T();
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

    template<
        typename T,
        typename = enable_if_t<is_object_v<T>, T>
    >
    explicit operator T();
};


// IMPLEMENTATION
// --------------


/** \brief Convert array view to STL container.
 */
template <typename T, typename>
ArrayView::operator T()
{
    typedef typename T::value_type U;

    T t;
    for (const auto &value: *this) {
        t.insert(t.end(), U(value));
    }

    return t;
}


/** \brief Convert object view to STL container.
 */
template <typename T, typename>
ObjectView::operator T()
{
    static_assert(is_object_v<T>, "");
    typedef typename T::key_type K;
    typedef typename T::mapped_type M;

    T t;
    for (const auto &pair: *this) {
        t.emplace(K(pair.first), M(pair.second));
    }

    return t;
}

}   /* json */
