//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup Lattice
 *  \brief Implementation of JSON TextReader.
 */

#include "json/reader.hpp"


namespace json
{
// OBJECTS
// -------


/** \brief Get depth of current reader.
 */
size_t TextReader::depth() const
{
    return nodes.size();
}


/** \brief Check if stream is at end of file.
 */
bool TextReader::isEof() const
{
    return stream->eof() || stream->peek() == EOF;
}


/** \brief Check if stream is bad.
 */
bool TextReader::isBad() const
{
    return stream->bad();
}


/** \brief Check if the reader is valid.
 *
 *  \warning Initialized streams are invalid until `read()` has been called.
 */
bool TextReader::isValid() const
{
    return !nodes.empty() && !(isBad() || isEof());
}


/** \brief Check if current node is array.
 */
bool TextReader::isArray() const
{
    if (nodes.empty()) {
        return false;
    }
    return nodes.back() == NodeType::ARRAY;
}


/** \brief Check if current node is object.
 */
bool TextReader::isArray() const
{
    if (nodes.empty()) {
        return false;
    }
    return nodes.back() == NodeType::OBJECT;
}

}   /* json */
