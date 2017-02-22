//  :copyright: (c) 2016-2017 The Regents of the University of California.
//  :license: MIT, see LICENSE.md for more details.
/**
 *  \addtogroup JSON
 *  \brief Type detection for C++ containers.
 */

#pragma once

#include <array>
#include <deque>
#include <limits>
#include <list>
#include <set>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace json
{
namespace detail
{
// TYPES
// -----

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

template <typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// SIMPLE TYPES
// ------------

template <typename T>
struct is_char
{
    enum {
        value = (is_same_v<char, T> || is_same_v<const char, T> || is_same_v<unsigned char, T> || is_same_v<const unsigned char, T>)
    };
};

template <typename T>
constexpr bool is_char_v = is_char<T>::value;

template <typename T>
struct is_bool
{
    enum {
        value = (is_same_v<bool, T> || is_same_v<const bool, T>)
    };
};

template <typename T>
constexpr bool is_bool_v = is_bool<T>::value;

template <typename T>
struct is_integer
{
    enum {
        value = (std::numeric_limits<T>::is_integer && !is_bool_v<T> && !is_char_v<T>)
    };
};

template <typename T>
constexpr bool is_integer_v = is_integer<T>::value;

template <typename T>
struct is_float: std::is_floating_point<T>
{};

template <typename T>
constexpr bool is_float_v = is_float<T>::value;

template <typename T>
struct is_cstr: std::integral_constant<
        bool,
        is_same_v<char const *, decay_t<T>> ||
        is_same_v<char *, decay_t<T>>
    >
{};

template <typename T>
constexpr bool is_cstr_v = is_cstr<T>::value;

template <typename T>
struct is_std_string: std::is_base_of<std::string, T>
{};

template <typename T>
constexpr bool is_std_string_v = is_std_string<T>::value;

template <typename T>
struct is_string: std::integral_constant<bool, is_cstr_v<T> || is_std_string_v<T>>
{};

template <typename T>
constexpr bool is_string_v = is_string<T>::value;

// COLLECTIONS
// -----------

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
struct is_array<std::list<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::list<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::set<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::set<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::multiset<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::multiset<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::unordered_set<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::unordered_set<Ts...>&>: std::true_type
{};

template <typename... Ts>
struct is_array<std::unordered_multiset<Ts...>>: std::true_type
{};

template <typename... Ts>
struct is_array<std::unordered_multiset<Ts...>&>: std::true_type
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

// TYPES
// -----


template <bool B, typename T = void>
using enable_if_t = detail::enable_if_t<B, T>;


//template <typename T, typename U>
//constexpr bool is_same_v = std::is_same<T, U>::value;
//is_char_v
//is_bool_v
//is_integer_v
//is_float_v
//is_cstr_v
//is_std_string_v

template <typename T>
constexpr bool is_string_v = detail::is_string_v<T>;

template <typename... Ts>
constexpr bool is_array_v = detail::is_array_v<Ts...>;

template <typename... Ts>
constexpr bool is_object_v = detail::is_object_v<Ts...>;

}   /* json */
