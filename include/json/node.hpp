//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup Json
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

}   /* json */
