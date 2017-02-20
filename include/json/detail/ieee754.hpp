//  :copyright: (c) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Floating point number specifications.
 */

#pragma once


namespace json
{
namespace detail
{
// FUNCTIONS
// ---------

/** \brief Check if number is NaN.
 */
bool isNan(const double number);

/** \brief Check if number is inf.
 */
bool isInf(const double number);

/** \brief Check if number is -inf.
 */
bool isNegativeInfinity(const double number);

/** \brief Check if number is inf.
 */
bool isInfinity(const double number);

/** \brief Check if number is not NaN or inf.
 */
bool isFinite(const double number);

/** \brief Check if number is zero.
 */
bool isZero(const double number);

/** \brief Check if number is positive.
 */
bool isPositive(const double number);

}   /* detail */
}   /* json */
