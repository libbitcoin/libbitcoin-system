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
#include <bitcoin/system/crypto/shamir/rs1024.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace shamir {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

data_slice rs1024::customize(bool extendable) NOEXCEPT
{
    if (extendable)
        return data_slice{ extended_customization };

    return data_slice{ customization };
}

rs1024::checksum_t rs1024::checksum(const words_t& words,
    bool extendable) NOEXCEPT
{
    // The residue is computed over the words and a zeroed checksum.
    auto padded = words;
    padded.resize(words.size() + checksum_words);
    const auto residue = bit_xor<uint32_t>(polymod(padded, extendable),
        bit_lo<uint32_t>);

    checksum_t out{};

    for (auto word = zero; word < checksum_words; ++word)
    {
        const auto shift = word_bits * (sub1(checksum_words) - word);
        const auto shifted = shift_right<uint32_t>(residue, shift);
        const auto masked = bit_and<uint32_t>(shifted, word_mask);
        out[word] = narrow_cast<word_t>(masked);
    }

    return out;
}

bool rs1024::verify(const words_t& words, bool extendable) NOEXCEPT
{
    return is_one(polymod(words, extendable));
}

uint32_t rs1024::polymod(const words_t& words, bool extendable) NOEXCEPT
{
    constexpr std_array<uint32_t, word_bits> generator
    {
        0x00e0e040, 0x01c1c080, 0x03838100, 0x07070200, 0x0e0e0009,
        0x1c0c2412, 0x38086c24, 0x3090fc48, 0x21b1f890, 0x03f3f120
    };

    const auto custom = customize(extendable);
    words_t values(custom.begin(), custom.end());
    values.insert(values.end(), words.cbegin(), words.cend());
    auto residue = bit_lo<uint32_t>;

    for (const auto value: values)
    {
        const auto high = shift_right<uint32_t>(residue, residue_bits);
        const auto low = bit_and<uint32_t>(residue, residue_mask);
        const auto shifted = shift_left<uint32_t>(low, word_bits);
        residue = bit_xor<uint32_t>(shifted, value);

        for (auto bit = zero; bit < word_bits; ++bit)
            if (get_right(high, bit))
                residue = bit_xor<uint32_t>(residue, generator[bit]);
    }

    return residue;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace shamir
} // namespace system
} // namespace libbitcoin
