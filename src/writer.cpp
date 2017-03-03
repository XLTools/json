//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Implementation of JSON TextWriter.
 */

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

// OBJECTS
// -------


/** \brief Check if argument node type matches current node type.
 */
void TextWriter::check_node(const NodeType type) const
{
    if (node.empty() || node.back() != type) {
        throw NodeError("NodeType does not match writer");
    }
}


void TextWriter::write_value_delimiter()
{
    if (offset.empty()) {
        return;
    } else if (offset.back() != 0) {
        stream->put(VALUE_DELIMITER);
    }
    ++offset.back();
}


void TextWriter::write_key_delimiter()
{
    stream->put(KEY_DELIMITER);
}


void TextWriter::open(std::ostream &stream)
{
    this->stream = &stream;
}


TextWriter::TextWriter(std::ostream &stream)
{
    open(stream);
}


size_t TextWriter::depth() const
{
    return node.size();
}


bool TextWriter::is_bad() const
{
    return stream->bad();
}


/** \warning Initialized streams are invalid until `start_array()` or
 *  `start_object()` has been called.
 */
bool TextWriter::is_valid() const
{
    return !node.empty() && !is_bad();
}


bool TextWriter::is_array() const
{
    if (node.empty()) {
        return false;
    }
    return node.back() == NodeType::ARRAY;
}


bool TextWriter::is_object() const
{
    if (node.empty()) {
        return false;
    }
    return node.back() == NodeType::OBJECT;
}


bool TextWriter::start_object()
{
    JSON_ACCEPTS_VALUE();
    // don't write a value delimiter if a key has been written
    if (!intermediate) {
        write_value_delimiter();
    } else {
        intermediate = false;
    }
    stream->put(START_OBJECT);
    node.emplace_back(NodeType::OBJECT);
    offset.emplace_back(0);

    return true;
}


bool TextWriter::start_object(const std::string &key)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    stream->write(key.data(), key.size());
    stream->put(KEY_DELIMITER);
    stream->put(START_OBJECT);
    node.emplace_back(NodeType::OBJECT);
    offset.emplace_back(0);

    return true;
}


bool TextWriter::end_object()
{
    JSON_ACCEPTS_KEY();

    stream->put(END_OBJECT);
    node.pop_back();
    offset.pop_back();
    if (!offset.empty()) {
        ++offset.back();
    }

    return true;
}


bool TextWriter::start_array()
{
    JSON_ACCEPTS_VALUE();
    // don't write a value delimiter if a key has been written
    if (!intermediate) {
        write_value_delimiter();
    } else {
        intermediate = false;
    }
    stream->put(START_ARRAY);
    node.emplace_back(NodeType::ARRAY);
    offset.emplace_back(0);

    return true;
}


bool TextWriter::start_array(const std::string &key)
{
    JSON_ACCEPTS_KEY();

    write_value_delimiter();
    stream->write(key.data(), key.size());
    stream->put(KEY_DELIMITER);
    stream->put(START_ARRAY);
    node.emplace_back(NodeType::ARRAY);
    offset.emplace_back(0);

    return true;
}


bool TextWriter::end_array()
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


FileTextWriter::FileTextWriter(const std::string &path):
    fstream(path, std::ios::out | std::ios::binary | std::ios::trunc)
{
    open(fstream);
}


StringTextWriter::StringTextWriter():
    sstream(std::ios::out | std::ios::binary)
{
    open(sstream);
}


std::string StringTextWriter::str() const
{
    return sstream.str();
}

}   /* json */
