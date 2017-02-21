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
                    throw ParserError("Invalid escape character.");
                    break;
            }
            escape = false;
            continue;
        }
        // check early exit
        if (c == '"') {
            break;
        }

        switch (c) {
            case '\\':
                escape = true;
                break;
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                throw ParserError("Unescaped control character in JSON string.");
            default:
                string += c;
                break;
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


/** \brief Validate the parsed input number.
 */
static void validateNumber(const std::string &string)
{
    if (string.empty()) {
        throw ParserError("Cannot have an empty number.");
    }

    // checking leading zeros
    bool minus = string.front() == '-';
    if (string.size() > (minus ? 2 : 1)) {
        if (minus) {
            if (string[1] == '0' && (string[2] >= '0' && string[2] <= '9')) {
                throw ParserError("String cannot have leading zeros.");
            }
        } else {
            if (string[0] == '0' && (string[1] >= '0' && string[1] <= '9')) {
                throw ParserError("String cannot have leading zeros.");
            }
        }
    }

    // check exponents
    const char f = string.front();
    const char b = string.back();
    switch (f) {
        case 'e':
        case 'E':
            throw ParserError("Incomplete exponent format.");
    }
    switch (b) {
        case 'e':
        case 'E':
            throw ParserError("Incomplete exponent format.");
        case '+':
        case '-':
            throw ParserError("Trailing arithmetic sign.");
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
    bool decimal = false;
    bool exponent = false;
    while (true) {
        c = stream.peek();
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                string += stream.get();
                break;
            }
            case '.': {
                if (string.empty() || string.back() < '0' || string.back() > '9') {
                    throw ParserError("Decimal must follow numerical values.");
                } else if (decimal) {
                    throw ParserError("Cannot have multiple decimals in value.");
                }
                decimal = true;
                string += stream.get();
                break;
            }
            case 'e':
            case 'E': {
                if (string.empty() || string.back() < '0' || string.back() > '9') {
                    throw ParserError("Exponent must follow numerical values.");
                } else if (exponent) {
                    throw ParserError("Cannot have multiple exponents in value.");
                }
                exponent = true;
                string += stream.get();
                break;
            }
            case '-': {
                if (!string.empty() && !(string.back() == 'e' || string.back() == 'E')) {
                    throw ParserError("- sign must follow exponent or be at start.");
                }
                string += stream.get();
                break;
            }
            case '+': {
                if (string.empty() || !(string.back() == 'e' || string.back() == 'E')) {
                    throw ParserError("+ sign must follow exponent.");
                }
                string += stream.get();
                break;
            }
            // CONTROL BLOCKS
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '}':
            case ']':
            case ',':
            case ':':
            case '\0':
            case EOF:
                type = ValueType::NUMBER;
                validateNumber(string);
                return;

            // ENDING CHARACTERS
            default:
                throw ParserError("Unrecognized values in number.");
        }
    }
}


// OBJECTS
// -------


/** \brief Initialize new object.
 */
void TextReaderImpl::startObject()
{
    node_.emplace_back(NodeType::OBJECT);
    offset_.emplace_back(0);
    type_ = ValueType::OBJECT_START;
    stream_->get();
}


/** \brief End object.
 */
void TextReaderImpl::endObject()
{
    checkEnd(node_, NodeType::OBJECT);
    node_.pop_back();
    offset_.pop_back();
    type_ = ValueType::OBJECT_END;
    if (!offset_.empty()) {
        ++offset_.back();
    }

    stream_->get();
}


/** \brief Initialize new array.
 */
void TextReaderImpl::startArray()
{
    node_.emplace_back(NodeType::ARRAY);
    offset_.emplace_back(0);
    type_ = ValueType::ARRAY_START;
    stream_->get();
}


/** \brief End array.
 */
void TextReaderImpl::endArray()
{
    checkEnd(node_, NodeType::ARRAY);
    node_.pop_back();
    offset_.pop_back();
    type_ = ValueType::ARRAY_END;
    if (!offset_.empty()) {
        ++offset_.back();
    }

    stream_->get();
}


/** \brief Increment offsets.
 */
void TextReaderImpl::increment()
{
    if (!offset_.empty()) {
        ++offset_.back();
    }
}


/** \brief Parse key from JSON reader.
 */
void TextReaderImpl::parseKey()
{
    switch (stream_->peek()) {
        case '"':
            parseString(*stream_, buffer_[0], type_);
            break;
        default:
            throw ParserError("Unexpected characters found for object key.");
    }
}


/** \brief Parse value from JSON reader.
 */
void TextReaderImpl::parseValue()
{
    switch (stream_->peek()) {
        case '}':
        case ']':
            throw ParserError("Unexpected ending characters.");
        case '{':
            startObject();
            break;
        case '[':
            startArray();
            break;
        case ',':
        case ':':
            throw ParserError("Unexpected character delimiters.");
        case '"':
            parseString(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'n':
            parseNull(*stream_, buffer_[1], type_);
            increment();
            break;
        case 't':
            parseTrue(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'f':
            parseFalse(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'i':
            parseInf(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'N':
            parseNan(*stream_, buffer_[1], type_);
            increment();
            break;
        default:
            parseNumber(*stream_, buffer_[1], type_);
            increment();
            break;
    }
}


/** \brief Parse key-value pair from an object.
 */
void TextReaderImpl::parsePair()
{
    parseKey();
    skipWhitespace(*stream_);
    if (!consume(*stream_, ':')) {
        throw ParserError("No : separating key-value pairs.");
    }
    skipWhitespace(*stream_);
    parseValue();
}


/** \brief Parse value from an array.
 */
void TextReaderImpl::parseArray()
{
     // sequential JSON items
    size_t offset = offset_.empty() ? 0 : offset_.back();
    switch (stream_->peek()) {
        case ',': {
            if (offset == 0) {
                throw ParserError("Cannot have comma after [ or {.");
            } else {
                // item is properly delimited by a comma
                stream_->get();
                skipWhitespace(*stream_);
                parseValue();
            }
            break;
        }
        case ']': {
            // end element should not have a comma before it.
            endArray();
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parseValue();
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


/** \brief Parse key-value pair from an object.
 */
void TextReaderImpl::parseObject()
{
    size_t offset = offset_.empty() ? 0 : offset_.back();
    switch (stream_->peek()) {
        case ',': {
            if (offset == 0) {
                throw ParserError("Cannot have comma after [ or {.");
            } else {
                // item is properly delimited by a comma
                stream_->get();
                skipWhitespace(*stream_);
                parsePair();
            }
            break;
        }
        case '}': {
            // end element should not have a comma before it.
            endObject();
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parsePair();
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


/** \brief Seek first element in buffer and parse first node.
 */
void TextReaderImpl::parseStart()
{
    skipWhitespace(*stream_);

    // validate first character
    switch (stream_->peek()) {
        case '\0':
        case EOF:
            throw ParserError("Empty JSON document\n");
        case '{':
            startObject();
            break;
        case '[':
            startArray();
            break;
        default: {
            // no root element -- RFC 7159 only
            parseValue();
            skipWhitespace(*stream_);
            const char c = stream_->peek();
            if (!(c == EOF || c == '\0')) {
                throw ParserError("Extra data.");
            }
        }
    }
}


/** \brief Parse item from stream.
 */
void TextReaderImpl::parseNode()
{
    buffer_[0].clear();
    buffer_[1].clear();
    skipWhitespace(*stream_);

    // if at the end of the file, break early
    switch (stream_->peek()) {
        case '\0':
        case EOF:
            return;
        default:
            break;
    }

    // parse remaining data
    if (!node_.empty()) {
        switch (node_.back()) {
            case NodeType::ARRAY:
                parseArray();
                return;
            case NodeType::OBJECT:
                parseObject();
                return;
        }
    }

    throw ParserError("Unexpected trailing characters from node.");
}


/** \brief Open text parser.
 */
void TextReaderImpl::open(std::istream &stream)
{
    stream_ = &stream;
    // reserve 1 Mb, to avoid reallocations
    buffer_[0].reserve(1048576);
    buffer_[1].reserve(1048576);
    parseStart();
}


/** \brief Initializer list constructor.
 */
TextReaderImpl::TextReaderImpl(std::istream &stream)
{
    open(stream);
}

}   /* detail */

// OBJECTS
// -------


/** \brief Initializer list constructor.
 */
TextReader::TextReader(std::istream &stream)
{
    TextReaderImpl::open(stream);
}


/** \brief Get depth of current reader.
 */
size_t TextReader::depth() const
{
    return node_.size();
}


/** \brief Check if stream is at end of file.
 */
bool TextReader::isEof() const
{
    return stream_->eof() || stream_->peek() == EOF;
}


/** \brief Check if stream is bad.
 */
bool TextReader::isBad() const
{
    return stream_->bad();
}


/** \brief Check if the reader is valid.
 *
 *  \warning Initialized streams are invalid until `read()` has been called.
 */
bool TextReader::isValid() const
{
    // !node_.empty() &&
    return !(isBad() || isEof());
}


/** \brief Check if current value is null.
 */
bool TextReader::isNull() const
{
    return type_ == ValueType::NULLPTR;
}


/** \brief Check if current value is bool.
 */
bool TextReader::isBool() const
{
    return type_ == ValueType::BOOLEAN;
}


/** \brief Check if current value is a number.
 */
bool TextReader::isNumber() const
{
    return type_ == ValueType::NUMBER;
}


/** \brief Check if current value is string.
 */
bool TextReader::isString() const
{
    return type_ == ValueType::STRING;
}


/** \brief Check if current node is array.
 */
bool TextReader::isArray() const
{
    if (node_.empty()) {
        return false;
    }
    return node_.back() == NodeType::ARRAY;
}


/** \brief Check if current node is object.
 */
bool TextReader::isObject() const
{
    if (node_.empty()) {
        return false;
    }
    return node_.back() == NodeType::OBJECT;
}


/** \brief Check if current node is a start node.
 */
bool TextReader::isStartNode() const
{
    switch (type_) {
        case ValueType::ARRAY_START:
        case ValueType::OBJECT_START:
            return true;
        default:
            return false;
    }
}


/** \brief Check if current node is an end node.
 */
bool TextReader::isEndNode() const
{
    switch (type_) {
        case ValueType::ARRAY_END:
        case ValueType::OBJECT_END:
            return true;
        default:
            return false;
    }
}


/** \brief Check if current node has key data.
 */
bool TextReader::hasKey() const
{
    return !buffer_[0].empty();
}


/** \brief Check if current node has value data.
 */
bool TextReader::hasValue() const
{
    return !buffer_[1].empty();
}


/** \brief Get current value type.
 */
ValueType TextReader::type() const
{
    return type_;
}


/** \brief Seek next element from parser.
 */
bool TextReader::read()
{
    if (isValid()) {
        parseNode();
    }
    if (!isValid() && !node_.empty()) {
        // unclosed item, after parsing finished
        throw ParserError("Unclosed `{` or `[` character.");
    }
    return true;
}


/** \brief Seek element at desired depth.
 */
bool TextReader::seek(const size_t depth)
{
    do {
        read();
    } while (this->depth() != depth && isValid());

    return this->depth() == depth;
}


/** \brief Seek element with the desired key name.
 */
bool TextReader::seek(const std::string &key)
{
    do {
        read();
    } while (buffer_[0] != key && isValid());

    return buffer_[0] == key;
}


/** \brief Seek element with the desired key name at the desired depth.
 */
bool TextReader::seek(const std::string &key,
    const size_t depth)
{
    do {
        read();
    } while (!(this->depth() == depth && buffer_[0] == key) && isValid());

    return this->depth() == depth;
}


/** \brief Get view of reader as an array for specialized parsing.
 */
ArrayView TextReader::array()
{
    return ArrayView(*this);
}


/** \brief Get view of reader as an object for specialized parsing.
 */
ObjectView TextReader::object()
{
    return ObjectView(*this);
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
