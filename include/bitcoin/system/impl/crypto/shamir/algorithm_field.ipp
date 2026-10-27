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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_FIELD_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_FIELD_IPP

// Field
// ============================================================================
// Multiplication is shift and reduce, with the addend and the reduction
// polynomial masked by the multiplier and carry bits. Inversion is
// exponentiation over the same multiplication, so neither the operands nor
// the share coordinates index memory or select a branch.

namespace libbitcoin {
namespace system {
namespace shamir {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr uint8_t CLASS::
multiply(uint8_t left, uint8_t right) NOEXCEPT
{
    auto multiplicand = left;
    auto multiplier = right;
    auto product = 0_u8;

    for (auto bit = zero; bit < byte_bits; ++bit)
    {
        const auto low = bit_and<uint8_t>(multiplier, bit_lo<uint8_t>);
        const auto addend = bit_and<uint8_t>(multiplicand, twos_complement(low));
        product = bit_xor<uint8_t>(product, addend);

        const auto high = shift_right<uint8_t>(multiplicand, sub1(byte_bits));
        const auto reduction = bit_and<uint8_t>(polynomial,
            twos_complement(high));
        const auto shifted = shift_left<uint8_t>(multiplicand);
        multiplicand = bit_xor<uint8_t>(shifted, reduction);

        multiplier = shift_right<uint8_t>(multiplier);
    }

    return product;
}

TEMPLATE
constexpr uint8_t CLASS::
invert(uint8_t value) NOEXCEPT
{
    auto power = value;
    auto inverse = bit_lo<uint8_t>;

    for (auto bit = zero; bit < byte_bits; ++bit)
    {
        if (get_right(inverse_power, bit))
            inverse = multiply(inverse, power);

        power = multiply(power, power);
    }

    return inverse;
}

TEMPLATE
typename CLASS::secret_t CLASS::
interpolate(uint8_t index, const shares_t& shares) NOEXCEPT
{
    secret_t out{};

    for (const auto& share: shares)
    {
        auto basis = bit_lo<uint8_t>;

        for (const auto& other: shares)
        {
            if (other.index == share.index)
                continue;

            const auto numerator = bit_xor<uint8_t>(index, other.index);
            const auto denominator = bit_xor<uint8_t>(share.index, other.index);
            const auto evaluation = multiply(numerator, invert(denominator));
            basis = multiply(basis, evaluation);
        }

        for (auto byte = zero; byte < secret_bytes; ++byte)
        {
            const auto term = multiply(share.value[byte], basis);
            out[byte] = bit_xor<uint8_t>(out[byte], term);
        }
    }

    return out;
}

} // namespace shamir
} // namespace system
} // namespace libbitcoin

#endif
