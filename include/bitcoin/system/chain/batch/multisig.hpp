/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BATCH_MULTISIG_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BATCH_MULTISIG_HPP

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Script helper for ecdsa multisig capture.
struct multisig
{
    /// Maximum keys/sigs for standard multisig capture.
    static constexpr auto maximum = power2(to_half(byte_bits));

    static constexpr bool check(size_t sigs, size_t keys) NOEXCEPT
    {
        return !is_zero(sigs) && !is_zero(keys) && !(keys > maximum) &&
            !(sigs > keys);
    }

    /// Banded expansion count: sig i pairs keys [i, i + (keys - sigs)].
    static constexpr size_t rows(size_t sigs, size_t keys) NOEXCEPT
    {
        const auto gap = keys - sigs;
        if (is_subtract_overflow(keys, sigs) || is_add_overflow(gap, one))
            return {};

        const auto sum = add1(gap);
        if (is_multiply_overflow(sigs, sum))
            return {};

        return sigs * sum;
    }
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
