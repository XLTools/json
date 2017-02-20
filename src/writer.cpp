//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Implementation of JSON TextWriter.
 */

#include "json/except.hpp"
#include "json/writer.hpp"

#include <array>
#include <string>


namespace json
{
// CHARACTERS
// ----------

const char START_ARRAY = '[';
const char END_ARRAY = ']';
const char START_OBJECT = '{';
const char END_OBJECT = '}';
const char VALUE_DELIMITER = ',';
const char KEY_DELIMITER = ':';
std::array<std::string, 2> INDENT = {"", "    "};
std::array<std::string, 2> SPACE = {"", " "};
std::array<std::string, 2> NEWLINE = {"", "\n"};

// MACROS
// ------

/** \brief Check the current element is an array element.
 */
#define JSON_CHECK_ARRAY()                                              \
    if (!isArray()) {                                                   \
        throw NodeError("TextWriter::isArray() -> false.");             \
    }

/** \brief Check the current element is an object element.
 */
#define JSON_CHECK_OBJECT()                                             \
    if (!isObject()) {                                                  \
        throw NodeError("TextWriter::isObject() -> false.");            \
    }

// OBJECTS
// -------


/** \brief Check if node type matches current node.
 */
void TextWriter::checkNode(const NodeType type) const
{
    if (node.empty() || node.back() != type) {
        throw NodeError("NodeType does not match writer");
    }
}


/** \brief Write value delimiter to start new node.
 */
void TextWriter::writeValueDelimiter()
{
    if (offset.empty()) {
        return;
    } else if (offset.back() != 0) {
        stream->put(VALUE_DELIMITER);
    }
    ++offset.back();
}


/** \brief Write key delimiter to allow key-value pairs.
 */
void TextWriter::writeKeyDelimiter()
{
    stream->put(KEY_DELIMITER);
}


/** \brief Open JSON text writer.
 */
void TextWriter::open(std::ostream &stream)
{
    this->stream = &stream;
}


/** \brief Initializer list constructor.
 */
TextWriter::TextWriter(std::ostream &stream)
{
    open(stream);
}


/** \brief Get depth of current reader.
 */
size_t TextWriter::depth() const
{
    return node.size();
}


/** \brief Check if stream is bad.
 */
bool TextWriter::isBad() const
{
    return stream->bad();
}


/** \brief Check if the reader is valid.
 *
 *  \warning Initialized streams are invalid until `startArray()` or
 *  `startObject()` has been called.
 */
bool TextWriter::isValid() const
{
    return !node.empty() && !isBad();
}


/** \brief Check if current node is array.
 */
bool TextWriter::isArray() const
{
    if (node.empty()) {
        return false;
    }
    return node.back() == NodeType::ARRAY;
}


/** \brief Check if current node is object.
 */
bool TextWriter::isObject() const
{
    if (node.empty()) {
        return false;
    }
    return node.back() == NodeType::OBJECT;
}


/** \brief Initialize an object element.
 */
bool TextWriter::startObject()
{
    // TODO: Need to think about the pretty writer
    stream->put(START_OBJECT);
    node.emplace_back(NodeType::OBJECT);
    offset.emplace_back(0);

    return true;
}


/** \brief End an object element.
 */
bool TextWriter::endObject()
{
    JSON_CHECK_OBJECT();

    stream->put(END_OBJECT);
    node.pop_back();
    offset.pop_back();
    if (!offset.empty()) {
        ++offset.back();
    }

    return true;
}


/** \brief Initialize an array element.
 */
bool TextWriter::startArray()
{
    // TODO: Need to think about the pretty writer
    stream->put(START_ARRAY);
    node.emplace_back(NodeType::ARRAY);
    offset.emplace_back(0);

    return true;
}


/** \brief End an array element.
 */
bool TextWriter::endArray()
{
    JSON_CHECK_ARRAY();

    stream->put(END_ARRAY);
    node.pop_back();
    offset.pop_back();
    if (!offset.empty()) {
        ++offset.back();
    }

    return true;
}


/** \brief Initialize from path.
 */
FileTextWriter::FileTextWriter(const std::string &path):
    fstream(path, std::ios::out | std::ios::binary | std::ios::trunc)
{
    open(fstream);
}


/** \brief Initialize from data.
 */
StringTextWriter::StringTextWriter():
    sstream(std::ios::out | std::ios::binary)
{
    open(sstream);
}


/** \brief Get string from stream.
 */
std::string StringTextWriter::str() const
{
    return sstream.str();
}


}   /* json */
