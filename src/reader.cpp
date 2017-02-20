//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Implementation of JSON TextReader.
 */

#include "json/except.hpp"
#include "json/reader.hpp"

#include <cstring>


namespace json
{
namespace detail
{
// CONSTANTS
// ---------

static const char WHITESPACE[] = " \n\r\t";
static const char START[] = "{[";
static const char END[] = "}]";
static const char VALUE_END[] = " \n\r\t}]";
static const char CONTROL[] = " \n\r\t}],:";


/** \brief Check if node end-type matches node start type.
 */
void checkEnd(std::deque<NodeType> &node,
    const NodeType type)
{
    if (node.empty() || node.back() != type) {
        throw std::invalid_argument("NodeType does not match reader");
    }
}


/** \brief Skip all whitespace for document.
 */
void skipWhitespace(std::istream &stream)
{
    while (strchr(WHITESPACE, stream.peek())) {
        stream.get();
    }
}


/** \brief Consume character and check if value is as expected.
 */
static bool consume(std::istream &stream,
    const char c)
{
    if (stream.peek() == c) {
        stream.get();
        return true;
    }
    return false;
}


/** \brief Parse string from file.
 */
static void parseString(TextReader &reader)
{
    bool escape = false;
    char c;
    while (true) {
        c = reader.stream().get();
        if (escape) {
            reader.buffer() += c;
            escape = false;
            continue;
        }
        if (c == '"') {
            break;
        } else if (c == '\\') {
            escape = true;
        } else {
            reader.buffer() += c;
        }
    }
    reader.type() = ValueType::STRING;
}


/** \brief Parse null value from file.
 */
static void parseNull(TextReader &reader)
{
    auto &is = reader.stream();
    if (consume(is, 'n') && consume(is, 'u') && consume(is, 'l') && consume(is, 'l')) {
        reader.type() = ValueType::NULLPTR;
        reader.buffer() = "null";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(is.tellg()));
    }
}


/** \brief Parse boolean true value from file.
 */
static void parseTrue(TextReader &reader)
{
    auto &is = reader.stream();
    if (consume(is, 't') && consume(is, 'r') && consume(is, 'u') && consume(is, 'e')) {
        reader.type() = ValueType::BOOLEAN;
        reader.buffer() = "true";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(is.tellg()));
    }
}


/** \brief Parse boolean false value from file.
 */
static void parseFalse(TextReader &reader)
{
    auto &is = reader.stream();
    if (consume(is, 'f') && consume(is, 'a') && consume(is, 'l') && consume(is, 's') && consume(is, 'e')) {
        reader.type() = ValueType::BOOLEAN;
        reader.buffer() = "false";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(is.tellg()));
    }
}


/** \brief Parse numeric inf value from file.
 */
static void parseInf(TextReader &reader)
{
    auto &is = reader.stream();
    if (consume(is, 'i') && consume(is, 'n') && consume(is, 'f')) {
        reader.type() = ValueType::NUMBER;
        reader.buffer() += "inf";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(is.tellg()));
    }
}


/** \brief Parse numeric NaN value from file.
 */
static void parseNan(TextReader &reader)
{
    auto &is = reader.stream();
    if (consume(is, 'N') && consume(is, 'a') && consume(is, 'N')) {
        reader.type() = ValueType::NUMBER;
        reader.buffer() = "NaN";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(is.tellg()));
    }
}


/** \brief Parse generic numeric value from file.
 */
static void parseNumber(TextReader &reader)
{
    // check negative alues
    bool minus = consume(reader.stream(), '-');
    if (minus) {
        reader.buffer() += '-';
        // can have a negative inf value, check
        if (reader.stream().peek() == 'i') {
            parseInf(reader);
            return;
        }
    }

    char c;
    while (true) {
        c = reader.stream().peek();
        if ((c >= '0' && c <= '9') || c == '.') {
            reader.buffer() += reader.stream().get();
        } else if (!strchr(CONTROL, c)) {
            throw ParserError("Unrecognized values in number.");
        } else {
            break;
        }
    }
    reader.type() = ValueType::NUMBER;
}


/** \brief Initialize new object.
 */
static void startObject(TextReader &reader)
{
    reader.node().emplace_back(NodeType::OBJECT);
    reader.offset().emplace_back(0);
    reader.type() = ValueType::OBJECT;
    reader.stream().get();
}


/** \brief End object.
 */
static void endObject(TextReader &reader)
{
    checkEnd(reader.node(), NodeType::OBJECT);
    reader.node().pop_back();
    reader.offset().pop_back();
    if (!reader.offset().empty()) {
        ++reader.offset().back();
    }

    reader.stream().get();
}


/** \brief Initialize new array.
 */
static void startArray(TextReader &reader)
{
    reader.node().emplace_back(NodeType::ARRAY);
    reader.offset().emplace_back(0);
    reader.type() = ValueType::ARRAY;
    reader.stream().get();
}


/** \brief End array.
 */
static void endArray(TextReader &reader)
{
    checkEnd(reader.node(), NodeType::ARRAY);
    reader.node().pop_back();
    reader.offset().pop_back();
    if (!reader.offset().empty()) {
        ++reader.offset().back();
    }

    reader.stream().get();
}


/** \brief Parse value from JSON reader.
 */
static void parseValue(TextReader &reader)
{
    switch (reader.stream().peek()) {
        case '}':
        case ']':
            throw ParserError("Unexpected ending characters.");
        case '{':
            startObject(reader);
            break;
        case '[':
            startArray(reader);
            break;
        case ',':
        case ':':
            throw ParserError("Unexpected character delimiters.");
        case '"':
            parseString(reader);
            break;
        case 'n':
            parseNull(reader);
            break;
        case 't':
            parseTrue(reader);
            break;
        case 'f':
            parseFalse(reader);
            break;
        case 'i':
            parseInf(reader);
            break;
        case 'N':
            parseNan(reader);
            break;
        default:
            parseNumber(reader);
            break;
    }
}


/** \brief Parse value from an array.
 */
static void parseArray(TextReader &reader)
{
     // sequential JSON items
    size_t offset = reader.offset().empty() ? 0 : reader.offset().back();
    switch (reader.stream().peek()) {
        case ',': {
            if (offset == 0) {
                throw ParserError("Cannot have comma after [ or {.");
            } else {
                // item is properly delimited by a comma
                reader.stream().get();
                skipWhitespace(reader.stream());
                parseValue(reader);
            }
            break;
        }
        case ']': {
            // end element should not have a comma before it.
            endArray(reader);
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parseValue(reader);
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


/** \brief Parse key-value pair from an object.
 */
static void parseObject(TextReader &reader)
{
    size_t offset = reader.offset().empty() ? 0 : reader.offset().back();
    switch (reader.stream().peek()) {
        case ',': {
            if (offset == 0) {
                throw ParserError("Cannot have comma after [ or {.");
            } else {
                // item is properly delimited by a comma
                // TODO: need to parse Key, then value
            }
        }
        case '}': {
            // end element should not have a comma before it.
            endObject(reader);
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                // TODO: need to parse Key, then value
                //parseValue(reader);
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


/** \brief Parse item from stream.
 */
static void parse(TextReader &reader)
{
    reader.buffer().clear();
    skipWhitespace(reader.stream());

    if (reader.isArray()) {
        parseArray(reader);
    } else if (reader.isObject()) {
        parseObject(reader);
    }
}


/** \brief Seek first element in buffer and parse first node.
 */
static void readStart(TextReader &reader)
{
    skipWhitespace(reader.stream());

    // validate first character
    switch (reader.stream().peek()) {
        case '\0':
        case EOF:
            throw ParserError("Empty JSON document\n");
        case '{':
            startObject(reader);
            break;
        case '[':
            startArray(reader);
            break;
        default:
            throw ParserError("Unexpected leading characters\n");
    }
}


}   /* detail */

// OBJECTS
// -------


/** \brief Initializer list constructor.
 */
TextReader::TextReader(std::istream &stream):
    stream_(&stream)
{
    // reserve 1 Mb, to avoid reallocations
    buffer().reserve(1048576);
    detail::readStart(*this);
}


/** \brief Get reference to stream.
 */
std::istream & TextReader::stream()
{
    return *stream_;
}


/** \brief Get const reference to stream.
 */
const std::istream & TextReader::stream() const
{
    return *stream_;
}


/** \brief Get reference to node.
 */
std::deque<NodeType> & TextReader::node()
{
    return node_;
}


/** \brief Get const reference to node.
 */
const std::deque<NodeType> & TextReader::node() const
{
    return node_;
}


/** \brief Get reference to offset.
 */
std::deque<size_t> & TextReader::offset()
{
    return offset_;
}


/** \brief Get const reference to offset.
 */
const std::deque<size_t> & TextReader::offset() const
{
    return offset_;
}


/** \brief Get reference to buffer.
 */
std::string & TextReader::buffer()
{
    return buffer_;
}


/** \brief Get const reference to buffer.
 */
const std::string & TextReader::buffer() const
{
    return buffer_;
}


/** \brief Get reference to current value type.
 */
ValueType & TextReader::type()
{
    return type_;
}


/** \brief Get const reference to current value type.
 */
const ValueType & TextReader::type() const
{
    return type_;
}


/** \brief Get depth of current reader.
 */
size_t TextReader::depth() const
{
    return node().size();
}


/** \brief Check if stream is at end of file.
 */
bool TextReader::isEof() const
{
    return stream().eof() || stream_->peek() == EOF;
}


/** \brief Check if stream is bad.
 */
bool TextReader::isBad() const
{
    return stream().bad();
}


/** \brief Check if the reader is valid.
 *
 *  \warning Initialized streams are invalid until `read()` has been called.
 */
bool TextReader::isValid() const
{
    return !node().empty() && !(isBad() || isEof());
}


/** \brief Check if current value is null.
 */
bool TextReader::isNull() const
{
    return type() == ValueType::NULLPTR;
}


/** \brief Check if current value is bool.
 */
bool TextReader::isBool() const
{
    return type() == ValueType::BOOLEAN;
}


/** \brief Check if current value is a number.
 */
bool TextReader::isNumber() const
{
    return type() == ValueType::NUMBER;
}


/** \brief Check if current value is string.
 */
bool TextReader::isString() const
{
    return type() == ValueType::STRING;
}


/** \brief Check if current node is array.
 */
bool TextReader::isArray() const
{
    if (node().empty()) {
        return false;
    }
    return node().back() == NodeType::ARRAY;
}


/** \brief Check if current node is object.
 */
bool TextReader::isObject() const
{
    if (node().empty()) {
        return false;
    }
    return node().back() == NodeType::OBJECT;
}


/** \brief Seek next element from parser.
 */
bool TextReader::read()
{
    detail::parse(*this);

    return true;
}

}   /* json */
