/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_NONCOPYABLE_HPP
#define LIBBITCOIN_SYSTEM_NONCOPYABLE_HPP

#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// Borrowing enable_if_t from c++14.
// en.cppreference.com/w/cpp/types/enable_if
template<bool Bool, class Type=void>
using enable_if_type = typename std::enable_if<Bool, Type>::type;

#define IS_DERIVED(Base, Type) \
enable_if_type<std::is_base_of<Base, Type>::value, bool>

#define IS_INTEGER(Type) \
enable_if_type<std::numeric_limits<Type>::is_integer, bool>

#define IS_UNSIGNED_INTEGER(Type) \
enable_if_type< \
    std::numeric_limits<Type>::is_integer && \
    !std::numeric_limits<Type>::is_signed, bool>

#define IS_SIGNED_INTEGER(Type) \
enable_if_type< \
    std::numeric_limits<Type>::is_integer && \
    std::numeric_limits<Type>::is_signed, bool>

#define IS_INTEGERS(Left, Right) \
enable_if_type< \
    std::numeric_limits<Left>::is_integer && \
    std::numeric_limits<Right>::is_integer, bool>

#define IS_UNSIGNED_INTEGERS(Left, Right) \
enable_if_type< \
    std::numeric_limits<Left>::is_integer && \
    !std::numeric_limits<Left>::is_signed && \
    std::numeric_limits<Right>::is_integer && \
    !std::numeric_limits<Right>::is_signed, bool>

#define IS_EITHER_INTEGER_SIGNED(Left, Right) \
enable_if_type< \
    (std::numeric_limits<Left>::is_integer && \
        std::numeric_limits<Left>::is_signed) || \
    (std::numeric_limits<Right>::is_integer && \
        std::numeric_limits<Right>::is_signed), bool>

// Derive from 'noncopyable' to preclude copy construct and assign semantics in
// the derived class. Move semantics are preserved if they are defined.
class BC_API noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() {}
};

} // namespace system
} // namespace libbitcoin

#endif
