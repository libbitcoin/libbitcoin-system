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
#ifndef LIBBITCOIN_SYSTEM_UINTX_DIVISION_IPP
#define LIBBITCOIN_SYSTEM_UINTX_DIVISION_IPP

#include <bit>

// based on:
// Knuth, The Art of Computer Programming, Vol. 2, 4.3.1, Algorithm D.

namespace libbitcoin {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

TEMPLATE
constexpr CLASS& CLASS::operator/=(const uintx_t& other) NOEXCEPT
{
    uintx_t remainder{};
    divide(*this, remainder, *this, other);
    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator%=(const uintx_t& other) NOEXCEPT
{
    uintx_t quotient{};
    divide(quotient, *this, *this, other);
    return *this;
}

// private
// Digits are 32 bits so that each step divides 64 by 32 bits.
TEMPLATE
constexpr void CLASS::divide(uintx_t& quotient, uintx_t& remainder,
    const uintx_t& dividend, const uintx_t& divisor) NOEXCEPT
{
    using digit = uint32_t;
    using wide = uint64_t;
    constexpr size_t digit_bits = std::numeric_limits<digit>::digits;
    constexpr size_t digits = limbs * 2u;
    constexpr wide base = wide{ 1 } << digit_bits;

    std::array<digit, digits> u{};
    std::array<digit, digits> v{};
    for (size_t index = 0; index < limbs; ++index)
    {
        u[2u * index] = static_cast<digit>(dividend.limbs_[index]);
        u[2u * index + 1u] = static_cast<digit>(dividend.limbs_[index] >>
            digit_bits);
        v[2u * index] = static_cast<digit>(divisor.limbs_[index]);
        v[2u * index + 1u] = static_cast<digit>(divisor.limbs_[index] >>
            digit_bits);
    }

    size_t n = digits;
    while (n > 0u && v[n - 1u] == 0u)
        --n;

    size_t m = digits;
    while (m > 0u && u[m - 1u] == 0u)
        --m;

    std::array<digit, digits> q{};
    std::array<digit, digits> r{};

    if (n == 0u)
    {
        // Division by zero produces zero.
    }
    else if (m < n)
    {
        r = u;
    }
    else if (n == 1u)
    {
        wide carry{};
        for (auto index = m; index > 0u; --index)
        {
            const auto current = (carry << digit_bits) | u[index - 1u];
            q[index - 1u] = static_cast<digit>(current / v.front());
            carry = current % v.front();
        }

        r.front() = static_cast<digit>(carry);
    }
    else
    {
        // D1: normalize so that the top divisor digit has its high bit set.
        const auto shift = static_cast<size_t>(std::countl_zero(v[n - 1u]));
        std::array<digit, digits> vn{};
        std::array<digit, digits + 1u> un{};
        for (auto index = n - 1u; index > 0u; --index)
            vn[index] = static_cast<digit>((v[index] << shift) |
                (shift == 0u ? 0u : (wide{ v[index - 1u] } >>
                    (digit_bits - shift))));

        vn.front() = static_cast<digit>(v.front() << shift);
        un[m] = static_cast<digit>(shift == 0u ? 0u :
            (wide{ u[m - 1u] } >> (digit_bits - shift)));

        for (auto index = m - 1u; index > 0u; --index)
            un[index] = static_cast<digit>((u[index] << shift) |
                (shift == 0u ? 0u : (wide{ u[index - 1u] } >>
                    (digit_bits - shift))));

        un.front() = static_cast<digit>(u.front() << shift);

        // D2-D7: each quotient digit from the top.
        for (auto j = m - n + 1u; j > 0u; --j)
        {
            const auto at = j - 1u;
            const auto top = (wide{ un[at + n] } << digit_bits) |
                un[at + n - 1u];

            auto qhat = top / vn[n - 1u];
            auto rhat = top % vn[n - 1u];
            while (qhat >= base || qhat * vn[n - 2u] >
                ((rhat << digit_bits) | un[at + n - 2u]))
            {
                --qhat;
                rhat += vn[n - 1u];
                if (rhat >= base)
                    break;
            }

            // D4: multiply and subtract.
            wide carry{};
            wide borrow{};
            for (size_t index = 0; index < n; ++index)
            {
                const auto product = qhat * vn[index] + carry;
                carry = product >> digit_bits;
                const auto low = static_cast<digit>(product);
                const auto value = wide{ un[at + index] } - low - borrow;
                un[at + index] = static_cast<digit>(value);
                borrow = (value >> digit_bits) != 0u ? 1u : 0u;
            }

            const auto value = wide{ un[at + n] } - carry - borrow;
            un[at + n] = static_cast<digit>(value);
            const auto negative = (value >> digit_bits) != 0u;

            // D5-D6: add back if the estimate was one too large.
            if (negative)
            {
                --qhat;
                wide sum{};
                for (size_t index = 0; index < n; ++index)
                {
                    sum += wide{ un[at + index] } + vn[index];
                    un[at + index] = static_cast<digit>(sum);
                    sum >>= digit_bits;
                }

                un[at + n] = static_cast<digit>(un[at + n] + sum);
            }

            q[at] = static_cast<digit>(qhat);
        }

        // D8: unnormalize the remainder.
        for (size_t index = 0; index < n; ++index)
            r[index] = static_cast<digit>((un[index] >> shift) |
                (shift == 0u ? 0u : (wide{ un[index + 1u] } <<
                    (digit_bits - shift))));
    }

    for (size_t index = 0; index < limbs; ++index)
    {
        quotient.limbs_[index] = (wide{ q[2u * index + 1u] } << digit_bits) |
            q[2u * index];
        remainder.limbs_[index] = (wide{ r[2u * index + 1u] } << digit_bits) |
            r[2u * index];
    }
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
