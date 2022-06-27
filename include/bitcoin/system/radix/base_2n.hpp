/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_2N_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_2N_HPP

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// base2n
// For unsigned and byte-aligned { span, base, precision } where
// [precision <= span / log2(base)] and representation is integral:

// These two functions are defined:
// [compress(span).exponent = clog(base, span)]
// [compress(span).mantissa = span * base^(exponent - precision)]]
// [expand(exponent, mantissa) = mantissa * base^(precision - exponent)]
// Where compression reduces span to precision.

// Substituting in either relation [e = precision +/- exponent]:
// [m * base^e]

// Substituting power2(log2(base)) for base, and <</>> for * power2:
// [m * (power2(log2(base)))^e] == [m << (log2(base) * abs(e))] for +e.
// [m * (power2(log2(base)))^e] == [m >> (log2(base) * abs(e))] for -e.

// Substituting factor for log2(base):
// [m * (power2(factor))^e] == [m << (factor * abs(e))] for +e.
// [m * (power2(factor))^e] == [m >> (factor * abs(e))] for -e.

// Substituting shift for factor * abs(e):
// [m * (power2(factor))^e] == [m << shift] for +e.
// [m * (power2(factor))^e] == [m >> shift] for -e.

// Substituting base for power2(log2(base)) = power2(factor):
// [m * base^e] == [m << shift] for +e.
// [m * base^e] == [m >> shift] for -e.

template <size_t Base = 256u, size_t Precision = 24u, size_t Span = 256u>
class base2n
{
public:
    static constexpr size_t base = Base;
    static constexpr size_t precision = Precision;
    static constexpr size_t span = Span;

    static constexpr size_t factor  = floored_log2(base);
    static constexpr size_t e_max   = span / factor;
    static constexpr size_t e_bits  = ceilinged_log2(e_max);
    static constexpr size_t e_bytes = to_ceilinged_bytes(e_bits);
    static constexpr size_t e_width = to_bits(e_bytes);
    static constexpr size_t m_bytes = precision / factor;

    using span_type = unsigned_exact_type<span / byte_bits>;
    using small_type = unsigned_type<m_bytes + e_bytes>;

    /// A zero value implies an invalid (including zero) parameter.
    /// Invalid if a padding bit is set. Allows non-minimal exponent encoding.
    static constexpr span_type expand(small_type exponential) noexcept;

    /// (m * ^e) bit-encoded as [00eeeee][mmmmmmmm][mmmmmmmm][mmmmmmmm].
    /// Highest two bits are padded with zeros, uses minimal exponent encoding.
    static constexpr small_type compress(const span_type& value) noexcept;

protected:
    template <typename Integer>
    static constexpr Integer raise(Integer exponent) noexcept
    {
        return exponent * factor;
    }

    template <typename Integer>
    static constexpr Integer lower(Integer exponent) noexcept
    {
        return exponent / factor;
    }

private:
    // Paramter constraints.
    static_assert(is_bytes_width(Span));
    static_assert(is_bytes_width(base));
    static_assert(is_bytes_width(precision));
    static_assert(is_integral<small_type>());
    static_assert(precision <= e_max);
};

/// chain::header.bits
using base256e = base2n<256, 24, 256>;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/radix/base_2n.ipp>

#endif
