/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ERROR_MACROS_HPP
#define LIBBITCOIN_SYSTEM_ERROR_MACROS_HPP

#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>

// The category parameter must be namespaced. The category name must correspond
// to the error type by the naming convention: category_t as the error type.
#define DECLARE_STD_ERROR_REGISTRATION(cat) \
namespace std { \
template <> \
struct is_error_code_enum<cat##_t> \
  : public true_type {}; \
template <> \
struct is_error_condition_enum<cat##_condition_t> \
  : public true_type {}; \
}

// For use of codes without conditions.
#define DECLARE_ERROR_T_CODE_CATEGORY(cat) \
enum cat##_condition_t {};\
class cat##_category \
  : public std::error_category \
{ \
    static const message_map<cat##_t> messages; \
public: \
    static const cat##_category singleton; \
    virtual const char* name() const noexcept; \
    virtual std::string message(int condition) const noexcept; \
    virtual std::error_condition default_error_condition(int value) const noexcept; \
}; \
std::error_code make_error_code(cat##_t value) noexcept; \
std::error_condition make_error_condition(cat##_t value) noexcept

// This adds equivalent() to DECLARE_ERROR_T_CODE_CATEGORY, which requires the
// corresponding DEFINE_ERROR_T_EQUIVALENCE. For use of codes with conditions.
#define DECLARE_ERROR_T_CONDITION_CATEGORY(cat) \
enum cat##_condition_t {};\
class cat##_category \
  : public std::error_category \
{ \
    static const message_map<cat##_t> messages; \
public: \
    static const cat##_category singleton; \
    virtual const char* name() const noexcept; \
    virtual std::string message(int condition) const noexcept; \
    virtual std::error_condition default_error_condition(int value) const noexcept; \
    virtual bool equivalent(const std::error_code& value, int condition) const noexcept; \
}; \
std::error_code make_error_code(cat##_t value) noexcept; \
std::error_condition make_error_condition(cat##_t value) noexcept

#define DEFINE_ERROR_T_CATEGORY(cat, category_name, unmapped) \
const cat##_category cat##_category::singleton; \
const char* cat##_category::name() const noexcept \
{ \
    return category_name; \
} \
std::string cat##_category::message(int condition) const noexcept \
{ \
    const auto message = messages.find(static_cast<cat##_t>(condition)); \
    return message == messages.end() ? unmapped : message->second; \
} \
std::error_condition cat##_category::default_error_condition(int code) const noexcept \
{ \
    return std::error_condition(code, *this); \
} \
std::error_code make_error_code(cat##_t value) noexcept \
{ \
    return std::error_code(value, cat##_category::singleton); \
} \
std::error_condition make_error_condition(cat##_t value) noexcept \
{ \
    return std::error_condition(value, cat##_category::singleton); \
}

// If an equivalence is not defined, codes are evaluated for equality.
// Despite registration of condition, it is never evaluated without equivalence.
// Define enumeration equivalence (if declared), return code.satisfies(condition).
#define DEFINE_ERROR_T_EQUIVALENCE(cat, code, condition) \
bool cat##_category::equivalent(const std::error_code& code, int condition) const noexcept

// Define enumeration mapping (always) to message text.
#define DEFINE_ERROR_T_MESSAGE_MAP(cat) \
const message_map<cat##_t> cat##_category::messages

template<typename Error>
using message_map = std::unordered_map<Error, std::string>;

// All about error codes and conditions here.
// blog.think-async.com/search/label/system_error

#endif
