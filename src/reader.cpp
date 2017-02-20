//  :copyright: (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.
//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Implementation of JSON TextReader.
 */

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
        throw NodeError("NodeType does not match reader");
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


/** \brief Convert code points to UTF-8.
 */
static std::string toUnicode(const size_t codepoint1,
    const size_t codepoint2 = 0)
{
    // calculate the code point from the given code points
    size_t codepoint = codepoint1;

    // check if codepoint1 is a high surrogate
    if (codepoint1 >= 0xD800 and codepoint1 <= 0xDBFF)
    {
        // check if codepoint2 is a low surrogate
        if (codepoint2 >= 0xDC00 and codepoint2 <= 0xDFFF) {
            // high surrogate occupies the most significant 22 bits
            // low surrogate occupies the least significant 15 bits
            codepoint = (codepoint1 << 10) + codepoint2 - 0x35FDC00;
        } else {
            throw ParserError("Invalid low surrogate.");
        }
    }

    std::string result;
    if (codepoint < 0x80) {
        // 1-byte characters: 0xxxxxxx (ASCII)
        result.append(1, static_cast<char>(codepoint));
    } else if (codepoint <= 0x7ff) {
        // 2-byte characters: 110xxxxx 10xxxxxx
        result.append(1, static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        result.append(1, static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xffff) {
        // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
        result.append(1, static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        result.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.append(1, static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10ffff) {
        // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        result.append(1, static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        result.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.append(1, static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        ParserError("code points above 0x10FFFF are invalid");
    }

    return result;
}


/** \brief Parse a \uxxxx or \uxxxx\uyyyy code point.
 */
static void parseCodepoint(std::istream &stream,
    std::string &string)
{
    // get \uxxxx code point
    char *data = new char[5];
    stream.read(data, 4);
    data[4] = '\0';
    auto codepoint = std::strtoul(data, nullptr, 16);
    delete[] data;

    // check if codepoint is a high surrogate
    if (codepoint >= 0xD800 and codepoint <= 0xDBFF) {
        // make sure there is a subsequent unicode
        if (!(consume(stream, '\\') && consume(stream, 'u'))) {
            throw ParserError("Missing low surrogate.");
        }

        // get \uyyyy code point and add codepoint to buffer
        data = new char[5];
        stream.read(data, 4);
        data[4] = '\0';
        auto codepoint2 = std::strtoul(data, nullptr, 16);
        delete[] data;
        string += toUnicode(codepoint, codepoint2);

    } else if (codepoint >= 0xDC00 and codepoint <= 0xDFFF) {
        // we found a lone low surrogate
        throw ParserError("Missing high surrogate.");
    } else {
        // add unicode character(s)
        string += toUnicode(codepoint);
    }
}


/** \brief Parse string from file.
 */
static void parseString(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (!consume(stream, '"')) {
        throw ParserError("String must begin with \".");
    }

    bool escape = false;
    char c;
    while (true) {
        c = stream.get();
        if (escape) {
            switch (c) {
                case 'n':
                    string += '\n';
                    break;
                case 't':
                    string += '\t';
                    break;
                case 'r':
                    string += '\r';
                    break;
                case 'f':
                    string += '\f';
                    break;
                case 'b':
                    string += '\b';
                    break;
                case 'u':
                    parseCodepoint(stream, string);
                    break;
                case '/':
                    string += '/';
                    break;
                case '\\':
                    string += '\\';
                    break;
                case '"':
                    string += '"';
                    break;
                default:
                    string += '\\';
                    string += c;
                    break;
            }
            escape = false;
            continue;
        }
        if (c == '"') {
            break;
        } else if (c == '\\') {
            escape = true;
        } else {
            string += c;
        }
    }
    type = ValueType::STRING;
}


/** \brief Parse null value from file.
 */
static void parseNull(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'n') && consume(stream, 'u') && consume(stream, 'l') && consume(stream, 'l')) {
        type = ValueType::NULLPTR;
        string = "null";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(stream.tellg()));
    }
}


/** \brief Parse boolean true value from file.
 */
static void parseTrue(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 't') && consume(stream, 'r') && consume(stream, 'u') && consume(stream, 'e')) {
        type = ValueType::BOOLEAN;
        string = "true";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(stream.tellg()));
    }
}


/** \brief Parse boolean false value from file.
 */
static void parseFalse(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'f') && consume(stream, 'a') && consume(stream, 'l') && consume(stream, 's') && consume(stream, 'e')) {
        type = ValueType::BOOLEAN;
        string = "false";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(stream.tellg()));
    }
}


/** \brief Parse numeric inf value from file.
 */
static void parseInf(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'i') && consume(stream, 'n') && consume(stream, 'f')) {
        type = ValueType::NUMBER;
        string += "inf";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(stream.tellg()));
    }
}


/** \brief Parse numeric NaN value from file.
 */
static void parseNan(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'N') && consume(stream, 'a') && consume(stream, 'N')) {
        type = ValueType::NUMBER;
        string = "NaN";
    } else {
        throw ParserError("Unknown stream value: " + std::to_string(stream.tellg()));
    }
}


/** \brief Parse generic numeric value from file.
 */
static void parseNumber(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    // check negative values
    bool minus = consume(stream, '-');
    if (minus) {
        string += '-';
        // can have a negative inf value, check
        if (stream.peek() == 'i') {
            parseInf(stream, string, type);
            return;
        }
    }

    char c;
    while (true) {
        c = stream.peek();
        if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-') {
            string += stream.get();
        } else if (!(strchr(CONTROL, c) || c == EOF || c == '\0')) {
            throw ParserError("Unrecognized values in number.");
        } else {
            break;
        }
    }
    type = ValueType::NUMBER;
}


/** \brief Initialize new object.
 */
static void startObject(TextReader &reader)
{
    reader.node().emplace_back(NodeType::OBJECT);
    reader.offset().emplace_back(0);
    reader.type() = ValueType::OBJECT_START;
    reader.stream().get();
}


/** \brief End object.
 */
static void endObject(TextReader &reader)
{
    checkEnd(reader.node(), NodeType::OBJECT);
    reader.node().pop_back();
    reader.offset().pop_back();
    reader.type() = ValueType::OBJECT_END;
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
    reader.type() = ValueType::ARRAY_START;
    reader.stream().get();
}


/** \brief End array.
 */
static void endArray(TextReader &reader)
{
    checkEnd(reader.node(), NodeType::ARRAY);
    reader.node().pop_back();
    reader.offset().pop_back();
    reader.type() = ValueType::ARRAY_END;
    if (!reader.offset().empty()) {
        ++reader.offset().back();
    }

    reader.stream().get();
}


/** \brief Parse key from JSON reader.
 */
static void parseKey(TextReader &reader)
{
    switch (reader.stream().peek()) {
        case '"':
            parseString(reader.stream(), reader.buffer()[0], reader.type());
            break;
        default:
            throw ParserError("Unexpected characters found for object key.");
    }
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
            parseString(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        case 'n':
            parseNull(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        case 't':
            parseTrue(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        case 'f':
            parseFalse(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        case 'i':
            parseInf(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        case 'N':
            parseNan(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
            break;
        default:
            parseNumber(reader.stream(), reader.buffer()[1], reader.type());
            if (!reader.offset().empty()) {
                ++reader.offset().back();
            }
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
static void parseObjectImpl(TextReader &reader)
{
    parseKey(reader);
    skipWhitespace(reader.stream());
    if (!consume(reader.stream(), ':')) {
        throw ParserError("No : separating key-value pairs.");
    }
    skipWhitespace(reader.stream());
    parseValue(reader);
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
                reader.stream().get();
                skipWhitespace(reader.stream());
                parseObjectImpl(reader);
            }
            break;
        }
        case '}': {
            // end element should not have a comma before it.
            endObject(reader);
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parseObjectImpl(reader);
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
    reader.buffer()[0].clear();
    reader.buffer()[1].clear();
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
        default: {
            // no root element -- RFC 7159 only
            parseValue(reader);
            skipWhitespace(reader.stream());
            const char c = reader.stream().peek();
            if (!(c == EOF || c == '\0')) {
                throw ParserError("Extra data.");
            }
        }
    }
}

}   /* detail */

// OBJECTS
// -------


/** \brief Open text parser.
 */
void TextReader::open(std::istream &stream)
{
    stream_ = &stream;
    // reserve 1 Mb, to avoid reallocations
    buffer()[0].reserve(1048576);
    buffer()[1].reserve(1048576);
    detail::readStart(*this);
}


/** \brief Initializer list constructor.
 */
TextReader::TextReader(std::istream &stream)
{
    open(stream);
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
std::array<std::string, 2> & TextReader::buffer()
{
    return buffer_;
}


/** \brief Get const reference to buffer.
 */
const std::array<std::string, 2> & TextReader::buffer() const
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


/** \brief Initialize from path.
 */
FileTextReader::FileTextReader(const std::string &path):
    fstream(path, std::ios::in | std::ios::binary)
{
    open(fstream);
}


/** \brief Initialize from data.
 */
StringTextReader::StringTextReader(const std::string &data):
    sstream(data, std::ios::in | std::ios::binary)
{
    open(sstream);
}

}   /* json */
