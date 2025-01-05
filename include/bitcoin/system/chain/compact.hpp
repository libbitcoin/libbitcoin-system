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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_COMPACT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_COMPACT_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A base 256 exponential form representation of 32 byte (uint256_t) values.
/// The zero value is used as an invalid value sentinel. Otherwise it is not
/// possible to create an invalid compact form from 32 bytes. The compression
/// loses all but the highest 24 (or 23) significant bits of precision. The
/// extra bit of precision loss is the only reason for this wrapper, as opposed
/// to a normal form base256 exponential representation. This implementation
/// imposes a stricter, (than satoshi) yet consensus-compliant, validation of
/// the exponential form.

struct compact
  : public base256e
{
public:
    /// A zero value implies an invalid (including zero) parameter.
    /// Non-minimal exponent encoding allowed only for mantissa sign bug.
    static constexpr span_type expand(small_type exponential) NOEXCEPT;

    /// (m * 256^e) bit-encoded as [0eeeeee][mmmmmmmm][mmmmmmmm][mmmmmmmm].
    /// Uses non-minimal exponent encoding to avoid mantissa sign (bug).
    static constexpr small_type compress(const span_type& number) NOEXCEPT;

protected:
    using exponent_type = unsigned_type<e_bytes>;

    struct parse
    {
        bool negative;
        exponent_type exponent;
        small_type mantissa;
    };

    static constexpr parse to_compact(small_type small) NOEXCEPT;
    static constexpr small_type from_compact(const parse& compact) NOEXCEPT;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/compact.ipp>

#endif
