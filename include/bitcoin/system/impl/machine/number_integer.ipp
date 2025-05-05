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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_INTEGER_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_INTEGER_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

// ****************************************************************************
// CONSENSUS:
// Due to compression, a leading sign byte is required if high bit is set.
// This produces two zero representations (+/-0). The subsequent byte-based
// overflow constraint thereby restricts the stack's 64-bit integer domain by
// one value. An implementation that either avoided vectorization or vector
// compression in the first place, or later implemented the overflow guard over
// the converted integeral would have avoided this seam. These were both
// premature optimizations, as a variant stack is more efficient than variably-
// lengthed byte vector integral storage. The resulting integer domain
// reduction is captured by the is_overflow functions below.
// ****************************************************************************

TEMPLATE
constexpr bool CLASS::
from_integer(Integer& out, int64_t vary) NOEXCEPT
{
    out = possible_narrow_cast<Integer>(vary);
    return !is_overflow(vary);
}

TEMPLATE
constexpr bool CLASS::
from_chunk(Integer& out, const data_chunk& vary) NOEXCEPT
{
    out = 0;

    // Just an optimization.
    if (strict_zero(vary))
        return true;

    if (is_overflow(vary))
        return false;

    // Size constraint guards from_little_endian.
    out = from_little_endian<Integer>(vary);

    // Restore sign from indication.
    if (is_negated(vary.back()))
        out = to_unnegated(out);

    return true;
}

// protected
TEMPLATE
constexpr bool CLASS::
strict_zero(const data_chunk& vary) NOEXCEPT
{
    return vary.empty();
}

// protected
TEMPLATE
constexpr bool CLASS::
is_overflow(const data_chunk& vary) NOEXCEPT
{
    return vary.size() > Size;
}

// protected
TEMPLATE
constexpr bool CLASS::
is_overflow(int64_t value) NOEXCEPT
{
    return is_limited(value, bitcoin_min<Size>, bitcoin_max<Size>);
}

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
