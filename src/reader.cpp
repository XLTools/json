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
void check_end(std::deque<NodeType> &node,
    const NodeType type)
{
    if (node.empty() || node.back() != type) {
        throw NodeError("NodeType does not match reader");
    }
}


void skip_whitespace(std::istream &stream)
{
    while (strchr(WHITESPACE, stream.peek())) {
        stream.get();
    }
}


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
static std::string to_unicode(const size_t codepoint1,
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
static void parse_codepoint(std::istream &stream,
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
        string += to_unicode(codepoint, codepoint2);

    } else if (codepoint >= 0xDC00 and codepoint <= 0xDFFF) {
        // we found a lone low surrogate
        throw ParserError("Missing high surrogate.");
    } else {
        // add unicode character(s)
        string += to_unicode(codepoint);
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
                    parse_codepoint(stream, string);
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


static void parse_null(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'n') && consume(stream, 'u') && consume(stream, 'l') && consume(stream, 'l')) {
        type = ValueType::NULLPTR;
        string += "null";
    } else {
        throw ParserError("Unknown stream value: " + lexi::lexi(stream.tellg()));
    }
}


static void parse_true(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 't') && consume(stream, 'r') && consume(stream, 'u') && consume(stream, 'e')) {
        type = ValueType::BOOLEAN;
        string += "true";
    } else {
        throw ParserError("Unknown stream value: " + lexi::lexi(stream.tellg()));
    }
}


static void parse_false(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'f') && consume(stream, 'a') && consume(stream, 'l') && consume(stream, 's') && consume(stream, 'e')) {
        type = ValueType::BOOLEAN;
        string += "false";
    } else {
        throw ParserError("Unknown stream value: " + lexi::lexi(stream.tellg()));
    }
}


static void parse_inf(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'I') && consume(stream, 'n') && consume(stream, 'f') && consume(stream, 'i') && consume(stream, 'n') && consume(stream, 'i') && consume(stream, 't') && consume(stream, 'y')) {
        type = ValueType::NUMBER;
        string += "Infinity";
    } else {
        throw ParserError("Unknown stream value: " + lexi::lexi(stream.tellg()));
    }
}


static void parse_nan(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    if (consume(stream, 'N') && consume(stream, 'a') && consume(stream, 'N')) {
        type = ValueType::NUMBER;
        string += "NaN";
    } else {
        throw ParserError("Unknown stream value: " + lexi::lexi(stream.tellg()));
    }
}


static void validate_number(const std::string &string)
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
static void parse_number(std::istream &stream,
    std::string &string,
    ValueType &type)
{
    // check negative values
    bool minus = consume(stream, '-');
    if (minus) {
        string += '-';
        // can have a negative inf value, check
        if (stream.peek() == 'I') {
            parse_inf(stream, string, type);
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
                validate_number(string);
                return;

            // ENDING CHARACTERS
            default:
                throw ParserError("Unrecognized values in number.");
        }
    }
}


// OBJECTS
// -------


void TextReaderImpl::start_object()
{
    node_.emplace_back(NodeType::OBJECT);
    offset_.emplace_back(0);
    type_ = ValueType::OBJECT_START;
    stream_->get();
}


void TextReaderImpl::end_object()
{
    check_end(node_, NodeType::OBJECT);
    node_.pop_back();
    offset_.pop_back();
    type_ = ValueType::OBJECT_END;
    if (!offset_.empty()) {
        ++offset_.back();
    }

    stream_->get();
}


void TextReaderImpl::start_array()
{
    node_.emplace_back(NodeType::ARRAY);
    offset_.emplace_back(0);
    type_ = ValueType::ARRAY_START;
    stream_->get();
}


void TextReaderImpl::end_array()
{
    check_end(node_, NodeType::ARRAY);
    node_.pop_back();
    offset_.pop_back();
    type_ = ValueType::ARRAY_END;
    if (!offset_.empty()) {
        ++offset_.back();
    }

    stream_->get();
}


void TextReaderImpl::increment()
{
    if (!offset_.empty()) {
        ++offset_.back();
    }
}


void TextReaderImpl::parse_key()
{
    switch (stream_->peek()) {
        case '"':
            parseString(*stream_, buffer_[0], type_);
            break;
        default:
            throw ParserError("Unexpected characters found for object key.");
    }
}


void TextReaderImpl::parse_value()
{
    switch (stream_->peek()) {
        case '}':
        case ']':
            throw ParserError("Unexpected ending characters.");
        case '{':
            start_object();
            break;
        case '[':
            start_array();
            break;
        case ',':
        case ':':
            throw ParserError("Unexpected character delimiters.");
        case '"':
            parseString(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'n':
            parse_null(*stream_, buffer_[1], type_);
            increment();
            break;
        case 't':
            parse_true(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'f':
            parse_false(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'I':
            parse_inf(*stream_, buffer_[1], type_);
            increment();
            break;
        case 'N':
            parse_nan(*stream_, buffer_[1], type_);
            increment();
            break;
        default:
            parse_number(*stream_, buffer_[1], type_);
            increment();
            break;
    }
}


void TextReaderImpl::parse_pair()
{
    parse_key();
    skip_whitespace(*stream_);
    if (!consume(*stream_, ':')) {
        throw ParserError("No : separating key-value pairs.");
    }
    skip_whitespace(*stream_);
    parse_value();
}


void TextReaderImpl::parse_array()
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
                skip_whitespace(*stream_);
                parse_value();
            }
            break;
        }
        case ']': {
            // end element should not have a comma before it.
            end_array();
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parse_value();
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


void TextReaderImpl::parse_object()
{
    size_t offset = offset_.empty() ? 0 : offset_.back();
    switch (stream_->peek()) {
        case ',': {
            if (offset == 0) {
                throw ParserError("Cannot have comma after [ or {.");
            } else {
                // item is properly delimited by a comma
                stream_->get();
                skip_whitespace(*stream_);
                parse_pair();
            }
            break;
        }
        case '}': {
            // end element should not have a comma before it.
            end_object();
            break;
        }
        default: {
            if (offset == 0) {
                // no preceding elements, no comma expected
                parse_pair();
            } else {
                throw ParserError("Expected ']' or ',' character.");
            }
        }
    }
}


void TextReaderImpl::parse_start()
{
    skip_whitespace(*stream_);

    // validate first character
    switch (stream_->peek()) {
        case '\0':
        case EOF:
            throw ParserError("Empty JSON document\n");
        case '{':
            start_object();
            break;
        case '[':
            start_array();
            break;
        default: {
            // no root element -- RFC 7159 only
            parse_value();
            skip_whitespace(*stream_);
            const char c = stream_->peek();
            if (!(c == EOF || c == '\0')) {
                throw ParserError("Extra data.");
            }
        }
    }
}


void TextReaderImpl::parse_node()
{
    buffer_[0].clear();
    buffer_[1].clear();
    skip_whitespace(*stream_);

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
                parse_array();
                return;
            case NodeType::OBJECT:
                parse_object();
                return;
        }
    }

    throw ParserError("Unexpected trailing characters from node.");
}


void TextReaderImpl::open(std::istream &stream)
{
    stream_ = &stream;
    // reserve 1 Mb, to avoid reallocations
    buffer_[0].reserve(1048576);
    buffer_[1].reserve(1048576);
    parse_start();
}


TextReaderImpl::TextReaderImpl(std::istream &stream)
{
    open(stream);
}

}   /* detail */

// OBJECTS
// -------


TextReader::TextReader(std::istream &stream)
{
    TextReaderImpl::open(stream);
}


size_t TextReader::depth() const
{
    return node_.size();
}


bool TextReader::is_eof() const
{
    return stream_->eof() || stream_->peek() == EOF;
}


bool TextReader::is_bad() const
{
    return stream_->bad();
}


/** \warning Initialized streams are invalid until `read()` has been called.
 */
bool TextReader::is_valid() const
{
    // !node_.empty() &&
    return !(is_bad() || is_eof());
}


bool TextReader::is_null() const
{
    return type_ == ValueType::NULLPTR;
}


bool TextReader::is_bool() const
{
    return type_ == ValueType::BOOLEAN;
}


bool TextReader::is_number() const
{
    return type_ == ValueType::NUMBER;
}


bool TextReader::is_string() const
{
    return type_ == ValueType::STRING;
}


bool TextReader::is_array() const
{
    if (node_.empty()) {
        return false;
    }
    return node_.back() == NodeType::ARRAY;
}


bool TextReader::is_object() const
{
    if (node_.empty()) {
        return false;
    }
    return node_.back() == NodeType::OBJECT;
}


bool TextReader::is_start_node() const
{
    switch (type_) {
        case ValueType::ARRAY_START:
        case ValueType::OBJECT_START:
            return true;
        default:
            return false;
    }
}


bool TextReader::is_end_node() const
{
    switch (type_) {
        case ValueType::ARRAY_END:
        case ValueType::OBJECT_END:
            return true;
        default:
            return false;
    }
}


bool TextReader::has_key() const
{
    return is_object() && !(is_start_node() || is_end_node());
}


bool TextReader::has_value() const
{
    if (buffer_[1].empty()) {
        return !(is_start_node() || is_end_node());
    }

    return true;
}


ValueType TextReader::type() const
{
    return type_;
}


bool TextReader::read()
{
    if (is_valid()) {
        parse_node();
    }
    if (!is_valid() && !node_.empty()) {
        // unclosed item, after parsing finished
        throw ParserError("Unclosed `{` or `[` character.");
    }
    return true;
}


bool TextReader::seek(const size_t depth)
{
    do {
        read();
    } while (this->depth() != depth && is_valid());

    return this->depth() == depth;
}


bool TextReader::seek(const std::string &key)
{
    do {
        read();
    } while (buffer_[0] != key && is_valid());

    return buffer_[0] == key;
}


bool TextReader::seek(const std::string &key,
    const size_t depth)
{
    do {
        read();
    } while (!(this->depth() == depth && buffer_[0] == key) && is_valid());

    return this->depth() == depth;
}


ArrayView TextReader::array()
{
    return ArrayView(*this);
}


ObjectView TextReader::object()
{
    return ObjectView(*this);
}


FileTextReader::FileTextReader(const std::string &path):
    fstream(path, std::ios::in | std::ios::binary)
{
    open(fstream);
}


StringTextReader::StringTextReader(const std::string &data):
    sstream(data, std::ios::in | std::ios::binary)
{
    open(sstream);
}

}   /* json */
