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
#ifndef LIBBITCOIN_SYSTEM_UINTX_ARITHMETIC_IPP
#define LIBBITCOIN_SYSTEM_UINTX_ARITHMETIC_IPP

#if defined(HAVE_MSC) && (defined(HAVE_X64) || defined(HAVE_ARM64))
    #include <intrin.h>
#endif

namespace libbitcoin {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Addition and subtraction carry across limbs, modulo 2^Bits.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS& CLASS::operator+=(const uintx_t& other) NOEXCEPT
{
    limb_t carry{};
    for (size_t index = 0; index < limbs; ++index)
    {
        const auto sum = limbs_[index] + other.limbs_[index];
        const auto total = sum + carry;
        carry = static_cast<limb_t>((sum < limbs_[index]) || (total < sum));
        limbs_[index] = total;
    }

    normalize();
    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator-=(const uintx_t& other) NOEXCEPT
{
    limb_t borrow{};
    for (size_t index = 0; index < limbs; ++index)
    {
        const auto difference = limbs_[index] - other.limbs_[index];
        const auto total = difference - borrow;
        borrow = static_cast<limb_t>((difference > limbs_[index]) ||
            (total > difference));
        limbs_[index] = total;
    }

    normalize();
    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator++() NOEXCEPT
{
    for (auto& limb: limbs_)
        if (++limb != 0u)
            break;

    normalize();
    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator--() NOEXCEPT
{
    for (auto& limb: limbs_)
        if (limb-- != 0u)
            break;

    normalize();
    return *this;
}

TEMPLATE
constexpr CLASS CLASS::operator++(int) NOEXCEPT
{
    auto copy = *this;
    ++(*this);
    return copy;
}

TEMPLATE
constexpr CLASS CLASS::operator--(int) NOEXCEPT
{
    auto copy = *this;
    --(*this);
    return copy;
}

// Multiplication is schoolbook, truncated to Bits.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS& CLASS::operator*=(const uintx_t& other) NOEXCEPT
{
    // Zero high limbs contribute nothing, so only used limbs are multiplied.
    const auto used = [](const limbs_t& value) NOEXCEPT
    {
        auto size = limbs;
        while (size > 0u && value[size - 1u] == 0u)
            --size;

        return size;
    };

    const auto left_size = used(limbs_);
    const auto right_size = used(other.limbs_);

    limbs_t product{};
    for (size_t left = 0; left < left_size; ++left)
    {
        limb_t carry{};
        const auto end = (limbs - left < right_size) ? limbs - left : right_size;
        for (size_t right = 0; right < end; ++right)
        {
            limb_t high{};
            limb_t low{};
            multiply(high, low, limbs_[left], other.limbs_[right]);

            auto& out = product[left + right];
            const auto sum = out + low;
            high += static_cast<limb_t>(sum < out);
            out = sum + carry;
            high += static_cast<limb_t>(out < sum);
            carry = high;
        }

        if (left + end < limbs)
            product[left + end] += carry;
    }

    limbs_ = product;
    normalize();
    return *this;
}

// private
TEMPLATE
constexpr void CLASS::multiply(limb_t& high, limb_t& low, limb_t left,
    limb_t right) NOEXCEPT
{
    if (!std::is_constant_evaluated())
    {
#if defined(HAVE_MSC) && defined(HAVE_X64)
        unsigned long long upper{};
        low = _umul128(left, right, &upper);
        high = upper;
        return;
#elif defined(HAVE_MSC) && defined(HAVE_ARM64)
        high = __umulh(left, right);
        low = left * right;
        return;
#elif defined(__SIZEOF_INT128__)
        const auto product = static_cast<unsigned __int128>(left) * right;
        high = static_cast<limb_t>(product >> limb_bits);
        low = static_cast<limb_t>(product);
        return;
#endif
    }

    constexpr auto half = limb_bits / 2u;
    constexpr auto mask = std::numeric_limits<limb_t>::max() >> half;
    const auto p0 = (left & mask) * (right & mask);
    const auto p1 = (left & mask) * (right >> half);
    const auto p2 = (left >> half) * (right & mask);
    const auto p3 = (left >> half) * (right >> half);
    const auto middle = (p0 >> half) + (p1 & mask) + (p2 & mask);
    low = (middle << half) | (p0 & mask);
    high = p3 + (p1 >> half) + (p2 >> half) + (middle >> half);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
