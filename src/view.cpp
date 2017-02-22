//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Object and array views for the TextReader.
 */

#include "json/view.hpp"


namespace json
{
// OBJECTS
// -------


/** \brief Initializer list constructor.
 */
ArrayView::ArrayView(TextReader &reader):
    reader(&reader)
{}


/** \brief Get iterator at beginning of array.
 */
ArrayIterator ArrayView::begin()
{
    return ArrayIterator(reader);
}


/** \brief Get iterator at null end of array.
 */
ArrayIterator ArrayView::end()
{
    return ArrayIterator();
}


/** \brief Initializer list constructor.
 */
ObjectView::ObjectView(TextReader &reader):
    reader(&reader)
{}


/** \brief Get iterator at beginning of object.
 */
ObjectIterator ObjectView::begin()
{
    return ObjectIterator(reader);
}


/** \brief Get iterator at null end of object.
 */
ObjectIterator ObjectView::end()
{
    return ObjectIterator();
}

}   /* json */
