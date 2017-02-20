//  :copyright: (c) 2016 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Type detection for C++ containers.
 */

#pragma once

#include <array>
#include <deque>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <vector>


namespace json
{
namespace detail
{
// TYPES
// -----

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// FUNCTIONS
// ---------

template <typename... Ts>
struct is_array: std::false_type
{};

template <typename... Ts>
struct is_array<std::array<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::array<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::vector<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::vector<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::deque<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::deque<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_object: std::false_type
{};

template <typename... Ts>
struct is_object<std::map<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_object<std::map<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_object<std::multimap<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_object<std::multimap<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_object<std::unordered_map<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_object<std::unordered_map<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_object<std::unordered_multimap<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_object<std::unordered_multimap<Ts...>&>: std::true_type
{};

template <typename... Ts>
constexpr bool is_array_v = is_array<Ts...>::value;

template <typename... Ts>
constexpr bool is_object_v = is_object<Ts...>::value;

}   /* detail */
}   /* json */
