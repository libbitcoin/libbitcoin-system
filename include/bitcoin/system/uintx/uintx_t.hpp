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
#ifndef LIBBITCOIN_SYSTEM_UINTX_UINTX_T_HPP
#define LIBBITCOIN_SYSTEM_UINTX_UINTX_T_HPP

#include <array>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <bitcoin/system/warnings.hpp>

namespace libbitcoin {

/// Integral operand of uintx_t (excludes bool).
template <typename Type>
constexpr bool is_uintx_integral = std::is_integral_v<Type> &&
    !std::is_same_v<Type, bool>;

template <typename Type>
using if_uintx_integral = std::enable_if_t<is_uintx_integral<Type>, bool>;

/// Floating point conversion target of uintx_t.
template <typename Type>
using if_uintx_floating = std::enable_if_t<std::is_floating_point_v<Type>,
    bool>;

/// Unsigned integer of fixed Bits width, with arithmetic modulo 2^Bits.
/// Integral operands are taken modulo 2^Bits, except that nonnegative
/// integrals compare by value. Division by zero is zero.
template <size_t Bits>
class uintx_t
{
public:
    static_assert(Bits > 0u);

    typedef uint64_t limb_t;
    static constexpr size_t limb_bits = std::numeric_limits<limb_t>::digits;
    static constexpr size_t limbs = (Bits + limb_bits - 1u) / limb_bits;
    typedef std::array<limb_t, limbs> limbs_t;

    /// Constructors.
    /// -----------------------------------------------------------------------

    constexpr uintx_t() NOEXCEPT = default;

    /// Negative values are taken modulo 2^Bits (two's complement).
    template <typename Integral, if_uintx_integral<Integral> = true>
    constexpr uintx_t(Integral value) NOEXCEPT;

    /// Hexadecimal with 0x prefix, otherwise decimal, negative with - prefix
    /// (two's complement), zero if invalid.
    explicit constexpr uintx_t(const std::string_view& text) NOEXCEPT;

    /// Implicit when widening, explicit (truncating) when narrowing.
    template <size_t Size>
    constexpr explicit(Size > Bits) uintx_t(
        const uintx_t<Size>& other) NOEXCEPT;

    /// Conversions.
    /// -----------------------------------------------------------------------

    /// Truncating conversion to an integral (low order bits).
    template <typename Integral, if_uintx_integral<Integral> = true>
    constexpr explicit operator Integral() const NOEXCEPT;

    /// Nearest floating point value (ties to even).
    template <typename Float, if_uintx_floating<Float> = true>
    explicit operator Float() const NOEXCEPT;

    /// True if nonzero.
    constexpr explicit operator bool() const NOEXCEPT;

    /// Truncating conversion to an integral or uintx_t.
    template <typename Type>
    constexpr Type convert_to() const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Little-endian limbs, high bits above Bits always zero.
    constexpr const limbs_t& data() const NOEXCEPT;

    /// The bit at index (zero beyond Bits).
    constexpr bool bit(size_t index) const NOEXCEPT;

    /// Index of the highest set bit (zero if zero).
    constexpr size_t msb() const NOEXCEPT;

    /// Decimal text.
    std::string str() const NOEXCEPT;

    /// Modifiers.
    /// -----------------------------------------------------------------------

    /// Set the bit at index (ignored beyond Bits).
    constexpr uintx_t& set(size_t index) NOEXCEPT;

    constexpr uintx_t& operator+=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator-=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator*=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator/=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator%=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator&=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator|=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator^=(const uintx_t& other) NOEXCEPT;
    constexpr uintx_t& operator<<=(size_t shift) NOEXCEPT;
    constexpr uintx_t& operator>>=(size_t shift) NOEXCEPT;
    constexpr uintx_t& operator++() NOEXCEPT;
    constexpr uintx_t& operator--() NOEXCEPT;
    constexpr uintx_t operator++(int) NOEXCEPT;
    constexpr uintx_t operator--(int) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    friend constexpr uintx_t operator+(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left += right;
    }

    friend constexpr uintx_t operator-(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left -= right;
    }

    friend constexpr uintx_t operator*(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left *= right;
    }

    friend constexpr uintx_t operator/(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left /= right;
    }

    friend constexpr uintx_t operator%(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left %= right;
    }

    friend constexpr uintx_t operator&(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left &= right;
    }

    friend constexpr uintx_t operator|(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left |= right;
    }

    friend constexpr uintx_t operator^(uintx_t left,
        const uintx_t& right) NOEXCEPT
    {
        return left ^= right;
    }

    friend constexpr uintx_t operator<<(uintx_t value, size_t shift) NOEXCEPT
    {
        return value <<= shift;
    }

    friend constexpr uintx_t operator>>(uintx_t value, size_t shift) NOEXCEPT
    {
        return value >>= shift;
    }

    friend constexpr uintx_t operator~(const uintx_t& value) NOEXCEPT
    {
        return complement(value);
    }

    friend constexpr uintx_t operator-(const uintx_t& value) NOEXCEPT
    {
        return ++complement(value);
    }

    friend constexpr uintx_t operator+(const uintx_t& value) NOEXCEPT
    {
        return value;
    }

    friend constexpr bool operator!(const uintx_t& value) NOEXCEPT
    {
        return !static_cast<bool>(value);
    }

    friend constexpr bool operator==(const uintx_t& left,
        const uintx_t& right) NOEXCEPT
    {
        return left.limbs_ == right.limbs_;
    }

    friend constexpr std::strong_ordering operator<=>(const uintx_t& left,
        const uintx_t& right) NOEXCEPT
    {
        return compare(left, right);
    }

    /// Narrower widths compare by value.
    template <size_t Size, std::enable_if_t<(Size < Bits), bool> = true>
    friend constexpr bool operator==(const uintx_t& left,
        const uintx_t<Size>& right) NOEXCEPT
    {
        return left == uintx_t{ right };
    }

    template <size_t Size, std::enable_if_t<(Size < Bits), bool> = true>
    friend constexpr std::strong_ordering operator<=>(const uintx_t& left,
        const uintx_t<Size>& right) NOEXCEPT
    {
        return compare(left, uintx_t{ right });
    }

    /// Negative integrals compare modulo 2^Bits, others by value.
    template <typename Integral, if_uintx_integral<Integral> = true>
    friend constexpr bool operator==(const uintx_t& left,
        Integral right) NOEXCEPT
    {
        return !exceeds(right) && left == uintx_t{ right };
    }

    template <typename Integral, if_uintx_integral<Integral> = true>
    friend constexpr std::strong_ordering operator<=>(const uintx_t& left,
        Integral right) NOEXCEPT
    {
        return exceeds(right) ? std::strong_ordering::less :
            compare(left, uintx_t{ right });
    }

    /// Decimal, or hexadecimal if the stream is so formatted.
    friend std::ostream& operator<<(std::ostream& stream,
        const uintx_t& value) NOEXCEPT
    {
        return write(stream, value);
    }

private:
    template <size_t>
    friend class uintx_t;

    static constexpr size_t top_bits = Bits % limb_bits;
    static constexpr limb_t top_mask = (top_bits == 0u) ?
        std::numeric_limits<limb_t>::max() :
        (limb_t{ 1 } << top_bits) - 1u;

    // construct.ipp
    constexpr void normalize() NOEXCEPT;

    // arithmetic.ipp
    static constexpr void multiply(limb_t& high, limb_t& low, limb_t left,
        limb_t right) NOEXCEPT;

    // bitwise.ipp
    static constexpr uintx_t complement(const uintx_t& value) NOEXCEPT;

    // compare.ipp
    static constexpr std::strong_ordering compare(const uintx_t& left,
        const uintx_t& right) NOEXCEPT;
    template <typename Integral>
    static constexpr bool exceeds(Integral value) NOEXCEPT;

    // division.ipp
    static constexpr void divide(uintx_t& quotient, uintx_t& remainder,
        const uintx_t& dividend, const uintx_t& divisor) NOEXCEPT;

    // text.ipp
    static std::ostream& write(std::ostream& stream,
        const uintx_t& value) NOEXCEPT;

    limbs_t limbs_{};
};

} // namespace libbitcoin

namespace std {

template <size_t Bits>
class numeric_limits<libbitcoin::uintx_t<Bits>>
{
    using type = libbitcoin::uintx_t<Bits>;

public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr int digits = static_cast<int>(Bits);
    static constexpr int digits10 = static_cast<int>(Bits * 30103u / 100000u);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr float_round_style round_style = round_toward_zero;

    static constexpr type min() noexcept { return {}; }
    static constexpr type lowest() noexcept { return {}; }
    static constexpr type max() noexcept { return ~type{}; }
    static constexpr type epsilon() noexcept { return {}; }
    static constexpr type round_error() noexcept { return {}; }
    static constexpr type infinity() noexcept { return {}; }
    static constexpr type quiet_NaN() noexcept { return {}; }
    static constexpr type signaling_NaN() noexcept { return {}; }
    static constexpr type denorm_min() noexcept { return {}; }
};

} // namespace std

#define TEMPLATE template <size_t Bits>
#define CLASS uintx_t<Bits>

#include <bitcoin/system/impl/uintx/arithmetic.ipp>
#include <bitcoin/system/impl/uintx/bitwise.ipp>
#include <bitcoin/system/impl/uintx/compare.ipp>
#include <bitcoin/system/impl/uintx/construct.ipp>
#include <bitcoin/system/impl/uintx/division.ipp>
#include <bitcoin/system/impl/uintx/text.ipp>

#undef CLASS
#undef TEMPLATE

#endif
