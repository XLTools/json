//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Node definitions for JSON objects and arrays.
 */

#pragma once

#include <cstdint>


namespace json
{
// ENUM
// ----


/** \brief Enumerations for node types.
 */
enum class NodeType: uint8_t
{
    ARRAY   = 0,
    OBJECT  = 1,
};


/** \brief Enumerations for node types.
 */
enum class ValueType: uint8_t
{
    NULLPTR         = 0,
    NUMBER          = 1,
    BOOLEAN         = 2,
    STRING          = 3,
    ARRAY_START     = 4,
    OBJECT_START    = 5,
    ARRAY_END       = 6,
    OBJECT_END      = 7,
};

}   /* json */
