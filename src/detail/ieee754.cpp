//  :copyright: (c) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Floating point number specifications.
 */

#include "json/detail/ieee754.hpp"

#include <cstdint>


namespace json
{
namespace detail
{
// CONSTANTS
// ---------

static const uint64_t EXPONENT_MASK = 0x7FF0000000000000ULL;
static const uint64_t SIGNIFICAND_MASK = 0x000FFFFFFFFFFFFFULL;
static const uint64_t SIGN_MASK = 0x8000000000000000ULL;
static const uint64_t INFINITY = 0x7FF0000000000000ULL;
static const uint64_t NEGATIVE_INFINITY = 0xFFF0000000000000ULL;

// FUNCTIONS
// ---------


static uint64_t TO_UINT64T(const double number)
{
    return static_cast<uint64_t>(number);
}


/** \brief Check if number is NaN.
 */
bool isNan(const double number)
{
    return !isFinite(number) && ((TO_UINT64T(number) & SIGNIFICAND_MASK) != 0);
}


/** \brief Check if number is inf.
 */
bool isInf(const double number)
{
    return !isFinite(number) && ((TO_UINT64T(number) & SIGNIFICAND_MASK) == 0);
}


/** \brief Check if number is -inf.
 */
bool isNegativeInfinity(const double number)
{
    return TO_UINT64T(number) == NEGATIVE_INFINITY;
}


/** \brief Check if number is inf.
 */
bool isInfinity(const double number)
{
    return TO_UINT64T(number) == INFINITY;
}


/** \brief Check if number is not NaN or inf.
 */
bool isFinite(const double number)
{
    return (TO_UINT64T(number) & EXPONENT_MASK) != EXPONENT_MASK;
}


/** \brief Check if number is zero.
 */
bool isZero(const double number)
{
    return (TO_UINT64T(number) & (EXPONENT_MASK | SIGNIFICAND_MASK)) == 0;
}


/** \brief Check if number is positive.
 */
bool isPositive(const double number)
{
    return (TO_UINT64T(number) & SIGN_MASK) == 0;
}

}   /* detail */
}   /* json */
