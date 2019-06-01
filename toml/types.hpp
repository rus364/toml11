//     Copyright Toru Niina 2017.
// Distributed under the MIT License.
#ifndef TOML11_TYPES_HPP
#define TOML11_TYPES_HPP
#include "datetime.hpp"
#include "string.hpp"
#include "traits.hpp"
#include "comments.hpp"
#include <vector>
#include <unordered_map>

namespace toml
{

template<typename Comment, // discard/preserve_comment
         template<typename ...> class Table, // map-like class
         template<typename ...> class Array> // vector-like class
class basic_value;

using character = char;
using key = std::string;

using boolean        = bool;
using integer        = std::int64_t;
using floating       = double; // "float" is a keyward, cannot use it here.
// the following stuffs are structs defined here, so aliases are not needed.
// - string
// - offset_datetime
// - offset_datetime
// - local_datetime
// - local_date
// - local_time

// default toml::value and default array/table
using value = basic_value<discard_comments, std::unordered_map, std::vector>;

enum class value_t : std::uint8_t
{
    empty           =  0,
    boolean         =  1,
    integer         =  2,
    floating        =  3,
    string          =  4,
    offset_datetime =  5,
    local_datetime  =  6,
    local_date      =  7,
    local_time      =  8,
    array           =  9,
    table           = 10,
};

template<typename charT, typename traits>
inline std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, value_t t)
{
    switch(t)
    {
        case value_t::boolean         : os << "boolean";         return os;
        case value_t::integer         : os << "integer";         return os;
        case value_t::floating        : os << "floating";        return os;
        case value_t::string          : os << "string";          return os;
        case value_t::offset_datetime : os << "offset_datetime"; return os;
        case value_t::local_datetime  : os << "local_datetime";  return os;
        case value_t::local_date      : os << "local_date";      return os;
        case value_t::local_time      : os << "local_time";      return os;
        case value_t::array           : os << "array";           return os;
        case value_t::table           : os << "table";           return os;
        case value_t::empty           : os << "empty";           return os;
        default                       : os << "unknown";         return os;
    }
}

template<typename charT = char,
         typename traits = std::char_traits<charT>,
         typename alloc = std::allocator<charT>>
inline std::basic_string<charT, traits, alloc> stringize(value_t t)
{
    std::basic_ostringstream<charT, traits, alloc> oss;
    oss << t;
    return oss.str();
}

namespace detail
{

// helper to define a type that represents a value_t value.
template<value_t V>
using value_t_constant = std::integral_constant<value_t, V>;

// meta-function that convertes from value_t to the exact toml type that corresponds to.
// It takes toml::basic_value type because array and table types depend on it.
template<value_t t, typename Value> struct enum_to_type                      {using type = void                      ;};
template<typename Value> struct enum_to_type<value_t::empty          , Value>{using type = void                      ;};
template<typename Value> struct enum_to_type<value_t::boolean        , Value>{using type = boolean                   ;};
template<typename Value> struct enum_to_type<value_t::integer        , Value>{using type = integer                   ;};
template<typename Value> struct enum_to_type<value_t::floating       , Value>{using type = floating                  ;};
template<typename Value> struct enum_to_type<value_t::string         , Value>{using type = string                    ;};
template<typename Value> struct enum_to_type<value_t::offset_datetime, Value>{using type = offset_datetime           ;};
template<typename Value> struct enum_to_type<value_t::local_datetime , Value>{using type = local_datetime            ;};
template<typename Value> struct enum_to_type<value_t::local_date     , Value>{using type = local_date                ;};
template<typename Value> struct enum_to_type<value_t::local_time     , Value>{using type = local_time                ;};
template<typename Value> struct enum_to_type<value_t::array          , Value>{using type = typename Value::array_type;};
template<typename Value> struct enum_to_type<value_t::table          , Value>{using type = typename Value::table_type;};

// meta-function that converts from an exact toml type to the enum that corresponds to.
template<typename T, typename Value>
struct type_to_enum : std::conditional<
    std::is_same<T, typename Value::array_type>::value, // if T == array_type,
    value_t_constant<value_t::array>,                   // then value_t::array
    typename std::conditional<                          // else...
        std::is_same<T, typename Value::table_type>::value, // if T == table_type
        value_t_constant<value_t::table>,               // then value_t::table
        value_t_constant<value_t::empty>                // else value_t::empty
        >::type
    >::type {};
template<typename Value> struct type_to_enum<boolean        , Value>: value_t_constant<value_t::boolean        > {};
template<typename Value> struct type_to_enum<integer        , Value>: value_t_constant<value_t::integer        > {};
template<typename Value> struct type_to_enum<floating       , Value>: value_t_constant<value_t::floating       > {};
template<typename Value> struct type_to_enum<string         , Value>: value_t_constant<value_t::string         > {};
template<typename Value> struct type_to_enum<offset_datetime, Value>: value_t_constant<value_t::offset_datetime> {};
template<typename Value> struct type_to_enum<local_datetime , Value>: value_t_constant<value_t::local_datetime > {};
template<typename Value> struct type_to_enum<local_date     , Value>: value_t_constant<value_t::local_date     > {};
template<typename Value> struct type_to_enum<local_time     , Value>: value_t_constant<value_t::local_time     > {};

// meta-function that checks the type T is the same as one of the toml::* types.
template<typename T, typename Value>
struct is_exact_toml_type : disjunction<
    std::is_same<T, boolean        >,
    std::is_same<T, integer        >,
    std::is_same<T, floating       >,
    std::is_same<T, string         >,
    std::is_same<T, offset_datetime>,
    std::is_same<T, local_datetime >,
    std::is_same<T, local_date     >,
    std::is_same<T, local_time     >,
    std::is_same<T, typename Value::array_type>,
    std::is_same<T, typename Value::table_type>
    >{};
template<typename T, typename V> struct is_exact_toml_type<T&, V>               : is_exact_toml_type<T, V>{};
template<typename T, typename V> struct is_exact_toml_type<T const&, V>         : is_exact_toml_type<T, V>{};
template<typename T, typename V> struct is_exact_toml_type<T volatile&, V>      : is_exact_toml_type<T, V>{};
template<typename T, typename V> struct is_exact_toml_type<T const volatile&, V>: is_exact_toml_type<T, V>{};

// meta-function that check type T is convertible to toml::* types
template<typename T, typename Value>
struct is_convertible_to_toml_value : disjunction<
    std::is_same<T, boolean>,                    // T is bool or
    std::is_integral<T>,                         // T is an integer or
    std::is_floating_point<T>,                   // T is a floating point or
    std::is_same<T, std::string>,                // T is std::string or
    std::is_same<T, toml::string>,               // T is toml::string or
    is_string_literal<T>,                        // T is "string literal" or
    std::is_same<T, toml::local_date>,           // T is local_date or
    std::is_same<T, toml::local_time>,           // T is local_time or
    std::is_same<T, toml::local_datetime>,       // T is local_datetime or
    std::is_same<T, toml::offset_datetime>,      // T is offset_datetime or
    std::is_same<T, std::chrono::system_clock::time_point>, // T is time_point or
    is_chrono_duration<T>,                       // T is a duration or
    std::is_same<T, typename Value::array_type>, // T is an array type or
    std::is_same<T, typename Value::table_type>  // T is an array type or
    >{};

// meta-function that returns value_t that represent the convertible type
template<typename T, typename Value>
struct convertible_toml_type_of : std::conditional<
    /* if   */ is_exact_toml_type<T, Value>::value,
    /* then */ value_t_constant<type_to_enum<T, Value>::value>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ std::is_integral<T>::value,
    /* then */ value_t_constant<value_t::integer>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ std::is_floating_point<T>::value,
    /* then */ value_t_constant<value_t::floating>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ std::is_same<T, std::string>::value,
    /* then */ value_t_constant<value_t::string>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ is_string_literal<T>::value,
    /* then */ value_t_constant<value_t::string>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ is_chrono_duration<T>::value,
    /* then */ value_t_constant<value_t::local_time>,
    /* else */ typename std::conditional<
    // ----------------------------------------------------------------------
    /* if   */ std::is_same<T, std::chrono::system_clock::time_point>::value,
    /* then */ value_t_constant<value_t::offset_datetime>,
    /* else */ value_t_constant<value_t::empty>
    // ----------------------------------------------------------------------
    >::type>::type>::type>::type>::type>::type>::type {};

} // detail
} // toml
#endif// TOML11_TYPES_H
