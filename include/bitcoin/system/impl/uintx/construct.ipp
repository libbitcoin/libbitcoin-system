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
#ifndef LIBBITCOIN_SYSTEM_UINTX_CONSTRUCT_IPP
#define LIBBITCOIN_SYSTEM_UINTX_CONSTRUCT_IPP

namespace libbitcoin {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Constructors.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Integral, if_uintx_integral<Integral>>
constexpr CLASS::uintx_t(Integral value) NOEXCEPT
{
    if constexpr (std::is_signed_v<Integral>)
    {
        const auto wide = static_cast<int64_t>(value);
        if (wide < 0)
            limbs_.fill(std::numeric_limits<limb_t>::max());

        limbs_.front() = static_cast<limb_t>(wide);
    }
    else
    {
        limbs_.front() = static_cast<limb_t>(value);
    }

    normalize();
}

TEMPLATE
constexpr CLASS::uintx_t(const std::string_view& text) NOEXCEPT
{
    constexpr std::string_view prefix{ "0x" };
    const auto negative = text.starts_with('-');
    const auto unsigned_text = negative ? text.substr(1u) : text;
    const auto hex = unsigned_text.starts_with(prefix) ||
        unsigned_text.starts_with("0X");
    const auto digits = hex ? unsigned_text.substr(prefix.size()) :
        unsigned_text;
    if (digits.empty())
        return;

    uintx_t value{};
    for (const auto character: digits)
    {
        int digit{};
        if (character >= '0' && character <= '9')
            digit = character - '0';
        else if (hex && character >= 'a' && character <= 'f')
            digit = character - 'a' + 10;
        else if (hex && character >= 'A' && character <= 'F')
            digit = character - 'A' + 10;
        else
            return;

        // Scaled by shifts so that the radix is not taken modulo 2^Bits.
        value = hex ? (value << 4u) : (value << 3u) + (value << 1u);
        value += uintx_t{ digit };
    }

    *this = negative ? -value : value;
}

TEMPLATE
template <size_t Size>
constexpr CLASS::uintx_t(const uintx_t<Size>& other) NOEXCEPT
{
    constexpr auto count = (limbs < uintx_t<Size>::limbs) ? limbs :
        uintx_t<Size>::limbs;

    for (size_t index = 0; index < count; ++index)
        limbs_[index] = other.limbs_[index];

    normalize();
}

// Conversions.
// ----------------------------------------------------------------------------

TEMPLATE
template <typename Integral, if_uintx_integral<Integral>>
constexpr CLASS::operator Integral() const NOEXCEPT
{
    using unsigned_t = std::make_unsigned_t<Integral>;
    return static_cast<Integral>(static_cast<unsigned_t>(limbs_.front()));
}

// The top 64 bits carry a sticky bit for all lower bits, which preserves the
// rounding decision of the hardware conversion from 64 bits.
TEMPLATE
template <typename Float, if_uintx_floating<Float>>
CLASS::operator Float() const NOEXCEPT
{
    if (!(*this))
        return Float{ 0 };

    const auto width = msb() + 1u;
    if (width <= limb_bits)
        return static_cast<Float>(limbs_.front());

    const auto shift = width - limb_bits;
    auto top = static_cast<limb_t>(*this >> shift);
    if (*this != ((*this >> shift) << shift))
        top |= 1u;

    return std::ldexp(static_cast<Float>(top), static_cast<int>(shift));
}

TEMPLATE
constexpr CLASS::operator bool() const NOEXCEPT
{
    for (const auto limb: limbs_)
        if (limb != 0u)
            return true;

    return false;
}

TEMPLATE
template <typename Type>
constexpr Type CLASS::convert_to() const NOEXCEPT
{
    return static_cast<Type>(*this);
}

// Properties.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr const typename CLASS::limbs_t& CLASS::data() const NOEXCEPT
{
    return limbs_;
}

// private
// ----------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::normalize() NOEXCEPT
{
    limbs_.back() &= top_mask;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
