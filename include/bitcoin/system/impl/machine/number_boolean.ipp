/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_BOOLEAN_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_BOOLEAN_IPP

#include <algorithm>
#include <iterator>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/number.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

constexpr bool boolean::
from_integer(int64_t vary) NOEXCEPT
{
    // Boolean is not overflow constrained.
    return to_bool(vary);
}

constexpr bool boolean::
from_chunk(const data_chunk& vary) NOEXCEPT
{
    // An optimization, also guards vector empty.
    if (strict_false(vary))
        return false;

    // Boolean is not overflow constrained (any length of bytes is valid).
    ////if (is_overflow(vary))
    ////    return false;

    if (!is_sign_byte(vary.back()))
        return true;

    // A logical zero is any +/- sequence of zero bytes (sign excluded above).
    return std::any_of(vary.begin(), std::prev(vary.end()), is_nonzero<uint8_t>);

    // Size constraint guards from_little_endian.
    // any_of optimizes by eliminating this conversion, allocation, and copy.
    ////return to_bool(from_little_endian<uintx>(vary));
}

constexpr bool boolean::
from_integer(bool& value, int64_t vary) NOEXCEPT
{
    // Boolean is not overflow constrained.
    value = to_bool(vary);

    // Minimal boolean must be zero or one.
    return is_boolean(vary);
}

constexpr bool boolean::
from_chunk(bool& value, const data_chunk& vary) NOEXCEPT
{
    if (strict_false(vary))
    {
        value = false;
        return true;
    }

    return from_integer(value, vary.front());
}

constexpr bool boolean::
from_chunk_strict(const data_chunk& vary) NOEXCEPT
{
    // True if not strictly false.
    return !strict_false(vary);
}

// protected
constexpr bool boolean::
strict_false(const data_chunk& vary) NOEXCEPT
{
    return vary.empty();
}

// protected
constexpr bool boolean::
is_sign_byte(uint8_t byte) NOEXCEPT
{
    return (byte == positive_sign_byte) || (byte == negative_sign_byte);
}

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
