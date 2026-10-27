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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_RS1024_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_RS1024_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// SLIP-0039 [Shamir's Secret-Sharing for Mnemonic Codes].
// github.com/satoshilabs/slips/blob/master/slip-0039.md

namespace libbitcoin {
namespace system {
namespace shamir {

/// The slip39 share word checksum, a Reed-Solomon codeword over GF(1024),
/// which detects any error in three words. This is not the bech32 checksum.
class BC_API rs1024
{
public:
    /// Share words are ten bit values, three of which are the checksum.
    static constexpr auto word_bits = 10_size;
    static constexpr auto checksum_words = 3_size;
    using word_t = uint16_t;
    using words_t = std_vector<word_t>;
    using checksum_t = std_array<word_t, checksum_words>;

    /// The customization string, which binds the codeword to the flag.
    static data_slice customize(bool extendable) NOEXCEPT;

    /// The checksum words of the share words, which exclude the checksum.
    static checksum_t checksum(const words_t& words, bool extendable) NOEXCEPT;

    /// True if the share words, which include the checksum, are checksummed.
    static bool verify(const words_t& words, bool extendable) NOEXCEPT;

protected:
    static constexpr auto checksum_bits = word_bits * checksum_words;
    static constexpr auto residue_bits = checksum_bits - word_bits;
    static constexpr auto word_mask = unmask_right<uint32_t>(word_bits);
    static constexpr auto residue_mask = unmask_right<uint32_t>(residue_bits);
    static constexpr char customization[] = "shamir";
    static constexpr char extended_customization[] = "shamir_extendable";

    static uint32_t polymod(const words_t& words, bool extendable) NOEXCEPT;
};

} // namespace shamir
} // namespace system
} // namespace libbitcoin

#endif
