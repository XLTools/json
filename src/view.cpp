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


ArrayView::ArrayView(TextReader &reader):
    reader(&reader)
{}


ArrayIterator ArrayView::begin()
{
    return ArrayIterator(reader);
}


ArrayIterator ArrayView::end()
{
    return ArrayIterator();
}


ObjectView::ObjectView(TextReader &reader):
    reader(&reader)
{}


ObjectIterator ObjectView::begin()
{
    return ObjectIterator(reader);
}


ObjectIterator ObjectView::end()
{
    return ObjectIterator();
}

}   /* json */
