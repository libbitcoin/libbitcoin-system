/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_VALUE_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_VALUE_HPP

#include <initializer_list>
#include <system_error>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {
namespace json {

/// The type of a json value.
enum class kind : uint8_t
{
    null,
    bool_,
    int64,
    uint64,
    double_,
    string,
    array,
    object
};

class value;
class value_ref;
class array;
class object;
class key_value_pair;

using string = std::string;

/// Customization tags for value_from/value_to (found by ADL tag_invoke).
struct value_from_tag {};
template <typename Type>
struct value_to_tag {};

template <typename Type>
value value_from(Type&& from) THROWS;
template <typename Type>
void value_from(Type&& from, value& to) THROWS;
template <typename Type>
Type value_to(const value& from) THROWS;

/// The exception thrown by json access and conversion failures.
class exception
  : public std::system_error
{
public:
    using std::system_error::system_error;
};

/// Throw the json error as json::exception.
[[noreturn]] BC_API void throw_error(const code& ec) THROWS;

/// Arithmetic types other than bool.
template <typename Type>
constexpr bool is_json_number = std::is_arithmetic_v<std::decay_t<Type>> &&
    !is_same_type<Type, bool>;

/// Types that value_ref converts by value_from.
template <typename Type>
constexpr bool is_json_convertible =
    !std::is_convertible_v<const std::decay_t<Type>&, std::string_view> &&
    !std::is_arithmetic_v<std::decay_t<Type>> &&
    !is_same_type<Type, std::nullptr_t> &&
    !is_same_type<Type, value> &&
    !is_same_type<Type, value_ref> &&
    !is_same_type<Type, std::initializer_list<value_ref>>;

/// A reference to a value source within an initializer list.
/// A list of two elements with a string first is a key/value pair, and a list
/// of only key/value pairs is an object, otherwise an array.
class BC_API value_ref
{
public:
    using init_list = std::initializer_list<value_ref>;

    /// The list is an object (all elements are key/value pairs).
    static bool maybe_object(init_list list) NOEXCEPT;
    static value make_value(init_list list) THROWS;
    static array make_array(init_list list) THROWS;
    static object make_object(init_list list) THROWS;

    value_ref(std::string_view text) NOEXCEPT;
    value_ref(const char* text) NOEXCEPT;
    value_ref(const string& text) NOEXCEPT;
    value_ref(string&& text) NOEXCEPT;
    value_ref(init_list list) NOEXCEPT;
    value_ref(std::nullptr_t) NOEXCEPT;
    value_ref(bool boolean) NOEXCEPT;
    value_ref(double number) NOEXCEPT;
    value_ref(const value& other) NOEXCEPT;
    value_ref(value&& other) NOEXCEPT;

    template <typename Integer, if_signed_integral_integer<Integer> = true>
    value_ref(Integer number) NOEXCEPT;
    template <typename Integer, if_unsigned_integral_integer<Integer> = true>
    value_ref(Integer number) NOEXCEPT;

    /// Any other type is converted by value_from (moved if an rvalue).
    template <typename Type, bool_if<is_json_convertible<Type>> = true>
    value_ref(const Type& other) NOEXCEPT;
    template <typename Type, bool_if<is_json_convertible<Type> &&
        !std::is_lvalue_reference_v<Type>> = true>
    value_ref(Type&& other) NOEXCEPT;

    bool is_key_value_pair() const NOEXCEPT;
    std::string_view get_string() const NOEXCEPT;
    value make_value() const THROWS;

private:
    template <typename Type>
    static value from_const(const void* source) THROWS;
    template <typename Type>
    static value from_rvalue(const void* source) THROWS;

    using factory = value(*)(const void*);

    enum class what : uint8_t
    {
        text,
        moved,
        list,
        null,
        boolean,
        int64,
        uint64,
        number,
        copied,
        function
    };

    struct converter
    {
        const void* source;
        factory make;
    };

    union
    {
        std::string_view text_;
        const string* moved_;
        init_list list_;
        bool boolean_;
        int64_t int64_;
        uint64_t uint64_;
        double number_;
        const value* copied_;
        converter converter_;
    };

    what what_;
};

/// A json array, a sequence of values.
class BC_API array
{
public:
    using value_type = value;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value&;
    using const_reference = const value&;
    using pointer = value*;
    using const_pointer = const value*;
    using iterator = value*;
    using const_iterator = const value*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// Constructors.
    array() NOEXCEPT;
    array(const array& other) THROWS;
    array(array&& other) NOEXCEPT;
    explicit array(size_t count) THROWS;
    array(size_t count, const value& fill) THROWS;
    array(std::initializer_list<value_ref> list) THROWS;
    ~array() NOEXCEPT;

    /// Assignment.
    array& operator=(const array& other) THROWS;
    array& operator=(array&& other) NOEXCEPT;
    array& operator=(std::initializer_list<value_ref> list) THROWS;

    /// Capacity.
    bool empty() const NOEXCEPT;
    size_t size() const NOEXCEPT;
    size_t capacity() const NOEXCEPT;
    void reserve(size_t count) THROWS;
    void shrink_to_fit() THROWS;

    /// Access (at throws json_out_of_range).
    value& at(size_t index) THROWS;
    const value& at(size_t index) const THROWS;
    value& operator[](size_t index) NOEXCEPT;
    const value& operator[](size_t index) const NOEXCEPT;
    value* if_contains(size_t index) NOEXCEPT;
    const value* if_contains(size_t index) const NOEXCEPT;
    value& front() NOEXCEPT;
    const value& front() const NOEXCEPT;
    value& back() NOEXCEPT;
    const value& back() const NOEXCEPT;
    value* data() NOEXCEPT;
    const value* data() const NOEXCEPT;

    /// Iteration.
    iterator begin() NOEXCEPT;
    const_iterator begin() const NOEXCEPT;
    const_iterator cbegin() const NOEXCEPT;
    iterator end() NOEXCEPT;
    const_iterator end() const NOEXCEPT;
    const_iterator cend() const NOEXCEPT;
    reverse_iterator rbegin() NOEXCEPT;
    const_reverse_iterator rbegin() const NOEXCEPT;
    const_reverse_iterator crbegin() const NOEXCEPT;
    reverse_iterator rend() NOEXCEPT;
    const_reverse_iterator rend() const NOEXCEPT;
    const_reverse_iterator crend() const NOEXCEPT;

    /// Modifiers.
    void clear() NOEXCEPT;
    void push_back(const value& element) THROWS;
    void push_back(value&& element) THROWS;
    template <typename ...Args>
    value& emplace_back(Args&&... args) THROWS;
    iterator insert(const_iterator position, const value& element) THROWS;
    iterator insert(const_iterator position, value&& element) THROWS;
    template <typename ...Args>
    iterator emplace(const_iterator position, Args&&... args) THROWS;
    iterator erase(const_iterator position) NOEXCEPT;
    iterator erase(const_iterator first, const_iterator last) NOEXCEPT;
    void pop_back() NOEXCEPT;
    void resize(size_t count) THROWS;
    void resize(size_t count, const value& fill) THROWS;
    void swap(array& other) NOEXCEPT;

    /// Element-wise equality.
    bool operator==(const array& other) const NOEXCEPT;

private:
    std::vector<value> values_;
};

/// A json object, members in insertion order with unique keys.
/// Erase moves the last member into the erased position, stable_erase does
/// not reorder. Lookup is indexed by hash above a small member count.
class BC_API object
{
public:
    using key_type = std::string_view;
    using mapped_type = value;
    using value_type = key_value_pair;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = key_value_pair&;
    using const_reference = const key_value_pair&;
    using pointer = key_value_pair*;
    using const_pointer = const key_value_pair*;
    using iterator = key_value_pair*;
    using const_iterator = const key_value_pair*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using init_list = std::initializer_list<
        std::pair<std::string_view, value_ref>>;

    /// Constructors.
    object() NOEXCEPT;
    object(const object& other) THROWS;
    object(object&& other) NOEXCEPT;
    object(init_list list) THROWS;
    ~object() NOEXCEPT;

    /// Assignment.
    object& operator=(const object& other) THROWS;
    object& operator=(object&& other) NOEXCEPT;
    object& operator=(init_list list) THROWS;

    /// Capacity.
    bool empty() const NOEXCEPT;
    size_t size() const NOEXCEPT;
    size_t capacity() const NOEXCEPT;
    void reserve(size_t count) THROWS;

    /// Lookup (at throws json_out_of_range, operator[] inserts null).
    value& at(std::string_view key) THROWS;
    const value& at(std::string_view key) const THROWS;
    value& operator[](std::string_view key) THROWS;
    iterator find(std::string_view key) NOEXCEPT;
    const_iterator find(std::string_view key) const NOEXCEPT;
    bool contains(std::string_view key) const NOEXCEPT;
    size_t count(std::string_view key) const NOEXCEPT;
    value* if_contains(std::string_view key) NOEXCEPT;
    const value* if_contains(std::string_view key) const NOEXCEPT;

    /// Iteration.
    iterator begin() NOEXCEPT;
    const_iterator begin() const NOEXCEPT;
    const_iterator cbegin() const NOEXCEPT;
    iterator end() NOEXCEPT;
    const_iterator end() const NOEXCEPT;
    const_iterator cend() const NOEXCEPT;
    reverse_iterator rbegin() NOEXCEPT;
    const_reverse_iterator rbegin() const NOEXCEPT;
    const_reverse_iterator crbegin() const NOEXCEPT;
    reverse_iterator rend() NOEXCEPT;
    const_reverse_iterator rend() const NOEXCEPT;
    const_reverse_iterator crend() const NOEXCEPT;

    /// Modifiers (emplace and insert do not replace an existing member).
    void clear() NOEXCEPT;
    template <typename Type>
    std::pair<iterator, bool> emplace(std::string_view key,
        Type&& element) THROWS;
    template <typename Type>
    std::pair<iterator, bool> insert_or_assign(std::string_view key,
        Type&& element) THROWS;
    template <typename Pair>
    std::pair<iterator, bool> insert(Pair&& pair) THROWS;
    void insert(init_list list) THROWS;
    iterator erase(const_iterator position) NOEXCEPT;
    size_t erase(std::string_view key) NOEXCEPT;
    iterator stable_erase(const_iterator position) NOEXCEPT;
    size_t stable_erase(std::string_view key) NOEXCEPT;
    void swap(object& other) NOEXCEPT;

    /// Equality of member sets (order independent).
    bool operator==(const object& other) const NOEXCEPT;

private:
    static constexpr size_t indexed = 16;
    static constexpr size_t npos = max_size_t;

    size_t locate(std::string_view key) const NOEXCEPT;
    iterator append(key_value_pair&& member) THROWS;
    void link(size_t position) NOEXCEPT;
    void reindex() THROWS;

    std::vector<key_value_pair> members_;
    std::unique_ptr<std::vector<size_t>> buckets_;
};

/// A json value.
class BC_API value
{
public:
    /// Constructors.
    value() NOEXCEPT;
    value(const value& other) THROWS;
    value(value&& other) NOEXCEPT;
    value(std::nullptr_t) NOEXCEPT;
    value(bool boolean) NOEXCEPT;
    value(double number) NOEXCEPT;
    value(std::string_view text) THROWS;
    value(const char* text) THROWS;
    value(const string& text) THROWS;
    value(string&& text) NOEXCEPT;
    value(const array& other) THROWS;
    value(array&& other) NOEXCEPT;
    value(const object& other) THROWS;
    value(object&& other) NOEXCEPT;
    value(std::initializer_list<value_ref> list) THROWS;
    ~value() NOEXCEPT;

    template <typename Integer, if_signed_integral_integer<Integer> = true>
    value(Integer number) NOEXCEPT;
    template <typename Integer, if_unsigned_integral_integer<Integer> = true>
    value(Integer number) NOEXCEPT;

    /// Assignment.
    value& operator=(const value& other) THROWS;
    value& operator=(value&& other) NOEXCEPT;
    value& operator=(std::initializer_list<value_ref> list) THROWS;
    value& operator=(std::nullptr_t) NOEXCEPT;
    value& operator=(bool boolean) NOEXCEPT;
    value& operator=(double number) NOEXCEPT;
    value& operator=(std::string_view text) THROWS;
    value& operator=(const char* text) THROWS;
    value& operator=(const string& text) THROWS;
    value& operator=(string&& text) NOEXCEPT;
    value& operator=(const array& other) THROWS;
    value& operator=(array&& other) NOEXCEPT;
    value& operator=(const object& other) THROWS;
    value& operator=(object&& other) NOEXCEPT;

    template <typename Integer, if_signed_integral_integer<Integer> = true>
    value& operator=(Integer number) NOEXCEPT;
    template <typename Integer, if_unsigned_integral_integer<Integer> = true>
    value& operator=(Integer number) NOEXCEPT;

    /// Kind.
    json::kind kind() const NOEXCEPT;
    bool is_null() const NOEXCEPT;
    bool is_bool() const NOEXCEPT;
    bool is_int64() const NOEXCEPT;
    bool is_uint64() const NOEXCEPT;
    bool is_double() const NOEXCEPT;
    bool is_number() const NOEXCEPT;
    bool is_string() const NOEXCEPT;
    bool is_array() const NOEXCEPT;
    bool is_object() const NOEXCEPT;
    bool is_primitive() const NOEXCEPT;
    bool is_structured() const NOEXCEPT;

    /// Checked access, throws json::exception (json_not_<kind>).
    bool& as_bool() THROWS;
    bool as_bool() const THROWS;
    int64_t& as_int64() THROWS;
    int64_t as_int64() const THROWS;
    uint64_t& as_uint64() THROWS;
    uint64_t as_uint64() const THROWS;
    double& as_double() THROWS;
    double as_double() const THROWS;
    string& as_string() THROWS;
    const string& as_string() const THROWS;
    array& as_array() THROWS;
    const array& as_array() const THROWS;
    object& as_object() THROWS;
    const object& as_object() const THROWS;

    /// Unchecked access (kind is asserted).
    bool& get_bool() NOEXCEPT;
    bool get_bool() const NOEXCEPT;
    int64_t& get_int64() NOEXCEPT;
    int64_t get_int64() const NOEXCEPT;
    uint64_t& get_uint64() NOEXCEPT;
    uint64_t get_uint64() const NOEXCEPT;
    double& get_double() NOEXCEPT;
    double get_double() const NOEXCEPT;
    string& get_string() NOEXCEPT;
    const string& get_string() const NOEXCEPT;
    array& get_array() NOEXCEPT;
    const array& get_array() const NOEXCEPT;
    object& get_object() NOEXCEPT;
    const object& get_object() const NOEXCEPT;

    /// Optional access (nullptr if not of kind).
    bool* if_bool() NOEXCEPT;
    const bool* if_bool() const NOEXCEPT;
    int64_t* if_int64() NOEXCEPT;
    const int64_t* if_int64() const NOEXCEPT;
    uint64_t* if_uint64() NOEXCEPT;
    const uint64_t* if_uint64() const NOEXCEPT;
    double* if_double() NOEXCEPT;
    const double* if_double() const NOEXCEPT;
    string* if_string() NOEXCEPT;
    const string* if_string() const NOEXCEPT;
    array* if_array() NOEXCEPT;
    const array* if_array() const NOEXCEPT;
    object* if_object() NOEXCEPT;
    const object* if_object() const NOEXCEPT;

    /// Member/element access, throws if not object/array or not found.
    value& at(std::string_view key) THROWS;
    const value& at(std::string_view key) const THROWS;
    value& at(size_t index) THROWS;
    const value& at(size_t index) const THROWS;

    /// Exact number conversion, otherwise json_not_exact (or not_number).
    template <typename Number, bool_if<is_json_number<Number>> = true>
    Number to_number() const THROWS;
    template <typename Number, bool_if<is_json_number<Number>> = true>
    Number to_number(code& ec) const NOEXCEPT;

    /// Replace with a default value of the kind.
    void emplace_null() NOEXCEPT;
    bool& emplace_bool() NOEXCEPT;
    int64_t& emplace_int64() NOEXCEPT;
    uint64_t& emplace_uint64() NOEXCEPT;
    double& emplace_double() NOEXCEPT;
    string& emplace_string() NOEXCEPT;
    array& emplace_array() NOEXCEPT;
    object& emplace_object() NOEXCEPT;

    void swap(value& other) NOEXCEPT;

    /// Equality (signed and unsigned integers compare by value).
    bool operator==(const value& other) const NOEXCEPT;

private:
    using inner = std::variant
    <
        std::nullptr_t,
        bool,
        int64_t,
        uint64_t,
        double,
        string,
        array,
        object
    >;

    inner inner_;
};

/// An object member.
class BC_API key_value_pair
{
public:
    key_value_pair(std::string_view key, json::value&& element) THROWS;
    key_value_pair(string&& key, json::value&& element) NOEXCEPT;

    std::string_view key() const NOEXCEPT;
    const char* key_c_str() const NOEXCEPT;
    json::value& value() NOEXCEPT;
    const json::value& value() const NOEXCEPT;

private:
    friend class object;

    string key_;
    json::value value_;
    size_t next_;
};

BC_API void swap(value& left, value& right) NOEXCEPT;
BC_API void swap(array& left, array& right) NOEXCEPT;
BC_API void swap(object& left, object& right) NOEXCEPT;

} // namespace json
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/serial/json/value.ipp>
#include <bitcoin/system/impl/serial/json/conversion.ipp>

#endif
