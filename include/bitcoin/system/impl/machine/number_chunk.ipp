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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_CHUNK_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_CHUNK_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

// Minimally-sized byte encoding, with extra allocated byte if negated.

inline data_chunk chunk::
from_bool(bool vary) NOEXCEPT
{
    static const data_chunk true_{ 1 };
    static const data_chunk false_{};
    return vary ? true_ : false_;
}

inline data_chunk chunk::
from_integer(int64_t vary) NOEXCEPT
{
    static const data_chunk empty{};

    // absolute(minimum<int64_t>) guarded by the presumption of int32 ops.
    BC_ASSERT(!is_negate_overflow(vary));

    // Just an optimization.
    if (is_zero(vary))
        return empty;

    const auto value = absolute(vary);
    const auto negated = is_negated(value);
    const auto negative = is_negative(vary);

    auto bytes = to_little_endian_size(value, to_int(negated));

    // Indicate the sign.
    if (negated && negative)
        bytes.push_back(negative_sign_byte);
    else if (negated)
        bytes.push_back(positive_sign_byte);
    else if (negative)
        bytes.back() = to_negated(bytes.back());

    return bytes;
}

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
