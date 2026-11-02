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
#ifndef LIBBITCOIN_SYSTEM_UINTX_TEXT_IPP
#define LIBBITCOIN_SYSTEM_UINTX_TEXT_IPP

#include <algorithm>
#include <ios>

namespace libbitcoin {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

TEMPLATE
std::string CLASS::str() const NOEXCEPT
{
    // Short division of 32 bit digits by the largest power of ten in 32 bits.
    constexpr uint64_t chunk = 1000000000;
    constexpr size_t chunk_digits = 9;
    constexpr size_t digit_bits = 32;

    std::array<uint32_t, limbs * 2u> digits{};
    for (size_t index = 0; index < limbs; ++index)
    {
        digits[2u * index] = static_cast<uint32_t>(limbs_[index]);
        digits[2u * index + 1u] = static_cast<uint32_t>(limbs_[index] >>
            digit_bits);
    }

    auto size = digits.size();
    while (size > 0u && digits[size - 1u] == 0u)
        --size;

    if (size == 0u)
        return "0";

    std::string out{};
    while (size > 0u)
    {
        uint64_t remainder{};
        for (auto index = size; index > 0u; --index)
        {
            const auto current = (remainder << digit_bits) | digits[index - 1u];
            digits[index - 1u] = static_cast<uint32_t>(current / chunk);
            remainder = current % chunk;
        }

        while (size > 0u && digits[size - 1u] == 0u)
            --size;

        for (size_t digit = 0; digit < chunk_digits; ++digit)
        {
            out.push_back(static_cast<char>('0' + remainder % 10u));
            remainder /= 10u;
            if (size == 0u && remainder == 0u)
                break;
        }
    }

    std::reverse(out.begin(), out.end());
    return out;
}

// private
TEMPLATE
std::ostream& CLASS::write(std::ostream& stream,
    const uintx_t& value) NOEXCEPT
{
    const auto flags = stream.flags();
    if ((flags & std::ios_base::basefield) != std::ios_base::hex)
        return stream << value.str();

    constexpr size_t nibble_bits = 4;
    const auto upper = (flags & std::ios_base::uppercase) != 0;
    std::string out{};
    auto copy = value;
    do
    {
        const auto nibble = static_cast<uint8_t>(copy.limbs_.front() & 0x0fu);
        out.push_back(nibble < 10u ? static_cast<char>('0' + nibble) :
            static_cast<char>((upper ? 'A' : 'a') + nibble - 10u));
        copy >>= nibble_bits;
    } while (copy);

    if ((flags & std::ios_base::showbase) != 0)
        out.append(upper ? "X0" : "x0");

    std::reverse(out.begin(), out.end());
    return stream << out;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
