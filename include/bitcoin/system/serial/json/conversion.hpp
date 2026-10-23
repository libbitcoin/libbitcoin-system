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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_CONVERSION_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_CONVERSION_HPP

#include <optional>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/json/value.hpp>

namespace libbitcoin {
namespace system {
namespace json {

/// value_from/value_to resolve in order:
/// - tag_invoke(value_from_tag, value&, const Type&) found by ADL, or
///   tag_invoke(value_to_tag<Type>, const value&) found by ADL.
/// - json value types, bool, numbers (exact), strings and nullptr_t.
/// - std::optional (null when empty), std::variant (first that converts),
///   std::monostate (null).
/// - maps with string keys (object), ranges (array), tuples (array).
/// Conversion failures throw json::exception.

template <typename Type>
value value_from(Type&& from) THROWS;

template <typename Type>
void value_from(Type&& from, value& to) THROWS;

template <typename Type>
Type value_to(const value& from) THROWS;

} // namespace json
} // namespace system
} // namespace libbitcoin

#endif
