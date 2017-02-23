//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Write types to stream.
 */

#include "lexi/lexi.hpp"
#include "json/detail/string.hpp"
#include "json/detail/write.hpp"


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------


/** \brief Write null value.
 */
std::ostream & write(std::ostream &stream,
    const std::nullptr_t nullp)
{
    stream.write("null", 4);
    return stream;
}


/** \brief Write bool.
 */
std::ostream & write(std::ostream &stream,
    const bool boolean)
{
    auto string = lexi::lexi(boolean);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write C-string.
 */
std::ostream & write(std::ostream &stream,
    const char *string)
{
    auto escaped = escape(std::string(string));
    stream.put('"');
    stream.write(escaped.data(), escaped.size());
    stream.put('"');

    return stream;
}


/** \brief Write string.
 */
std::ostream & write(std::ostream &stream,
    const std::string &string)
{
    auto escaped = escape(string);
    stream.put('"');
    stream.write(escaped.data(), escaped.size());
    stream.put('"');

    return stream;
}


/** \brief Write character.
 */
std::ostream & write(std::ostream &stream,
    const char character)
{
    stream.put('"');
    stream.put(character);
    stream.put('"');

    return stream;
}


/** \brief Write character.
 */
std::ostream & write(std::ostream &stream,
    const unsigned char character)
{
    return write(stream, static_cast<char>(character));
}


/** \brief Write float.
 */
std::ostream & write(std::ostream &stream,
    const float number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write double.
 */
std::ostream & write(std::ostream &stream,
    const double number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int16_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint16_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int32_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint32_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const int64_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write integer.
 */
std::ostream & write(std::ostream &stream,
    const uint64_t number)
{
    auto string = lexi::lexi(number);
    stream.write(string.data(), string.size());

    return stream;
}


/** \brief Write null key.
 */
std::ostream & writeKey(std::ostream &stream,
    const std::nullptr_t nullp)
{
    stream.put('"');
    write(stream, nullp);
    stream.put('"');

    return stream;
}

/** \brief Write bool.
 */
std::ostream & writeKey(std::ostream &stream,
    const bool boolean)
{
    stream.put('"');
    write(stream, boolean);
    stream.put('"');

    return stream;
}

/** \brief Write C-string.
 */
std::ostream & writeKey(std::ostream &stream,
    const char *string)
{
    return write(stream, string);
}

/** \brief Write string.
 */
std::ostream & writeKey(std::ostream &stream,
    const std::string &string)
{
    return write(stream, string);
}

/** \brief Write character.
 */
std::ostream & writeKey(std::ostream &stream,
    const char character)
{
    return write(stream, character);
}

/** \brief Write unsigned character.
 */
std::ostream & writeKey(std::ostream &stream,
    const unsigned char character)
{
    return write(stream, character);
}

/** \brief Write float.
 */
std::ostream & writeKey(std::ostream &stream,
    const float number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write double.
 */
std::ostream & writeKey(std::ostream &stream,
    const double number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int16_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint16_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int32_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint32_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const int64_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}

/** \brief Write integer.
 */
std::ostream & writeKey(std::ostream &stream,
    const uint64_t number)
{
    stream.put('"');
    write(stream, number);
    stream.put('"');

    return stream;
}


}   /* detail */
}   /* json */
