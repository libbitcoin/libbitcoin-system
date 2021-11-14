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
#include <unordered_map>

#define DECLARE_ERROR_T
#ifdef DECLARE_ERROR_T

#define DECLARE_ERROR_T_CATEGORY(code) \
enum code##_condition_t {};\
class code##_category \
  : public std::error_category \
{ \
    static const message_map<code##_t> messages; \
public: \
    static const code##_category singleton; \
    virtual const char* name() const noexcept; \
    virtual std::string message(int value) const noexcept; \
    virtual std::error_condition default_error_condition( \
        int value) const noexcept; \
}

#define DECLARE_STD_ERROR_T_REGISTRATION(space, code) \
namespace std { \
template <> \
struct is_error_code_enum<space::code##_t> \
  : public true_type {}; \
template <> \
struct is_error_condition_enum<space::code##_condition_t> \
  : public true_type {}; \
std::error_code make_error_code(space::code##_t value) noexcept; \
std::error_condition make_error_condition(space::code##_t value) noexcept; \
}

#endif // DECLARE_ERROR_T

#define DEFINE_ERROR_T
#ifdef DEFINE_ERROR_T

#define DEFINE_ERROR_T_CATEGORY(code, category, unmapped) \
const code##_category code##_category::singleton; \
const char* code##_category::name() const noexcept \
{ \
    return category; \
} \
std::string code##_category::message(int value) const noexcept \
{ \
    const auto message = messages.find(static_cast<code##_t>(value)); \
    return message == messages.end() ? unmapped : message->second; \
} \
std::error_condition code##_category::default_error_condition( \
    int value) const noexcept \
{ \
    return std::error_condition(value, *this); \
}

#define DEFINE_STD_ERROR_T_REGISTRATION(space, code) \
using namespace space; \
namespace std { \
std::error_code make_error_code(code##_t value) noexcept \
{ \
    return std::error_code(value, space::code##_category::singleton); \
} \
std::error_condition make_error_condition(code##_t value) noexcept \
{ \
    return std::error_condition(value, space::code##_category::singleton); \
} \
}

#endif // DEFINE_ERROR_T

#define DEFINE_ERROR_T_MESSAGE_MAP(code) \
const message_map<code##_t> code##_category::messages

template<typename Error>
using message_map = std::unordered_map<Error, std::string>;

#endif
