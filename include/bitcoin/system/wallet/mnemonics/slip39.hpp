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
#ifndef LIBBITCOIN_SYSTEM_WALLET_MNEMONICS_SLIP39_HPP
#define LIBBITCOIN_SYSTEM_WALLET_MNEMONICS_SLIP39_HPP

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/wallet/context.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/words/words.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A share of a master secret, as defined by SLIP39.
/// Splits a master secret into groups of shares, each share a mnemonic.
/// Recovers the master secret from a threshold of groups of shares.
/// The share value is the mnemonic entropy, and the master secret is the
/// "master binary seed" from which master keys are derived.
class BC_API slip39
  : public words::languages
{
public:
    DEFAULT_COPY_MOVE(slip39);

    typedef words::slip39::catalog dictionary;
    typedef words::slip39::catalogs dictionaries;
    typedef std_vector<slip39> list;

    /// A group of count shares, of which threshold are required.
    struct group
    {
        uint8_t threshold;
        uint8_t count;
    };

    typedef std_vector<group> groups;

    /// Supports 128 to 512 bits of master secret, in multiples of 16 bits.
    static constexpr size_t entropy_minimum = bytes<128>;
    static constexpr size_t entropy_maximum = bytes<512>;
    static constexpr size_t entropy_multiple = bytes<16>;

    /// Share indexes, thresholds and counts are four bit values, so there
    /// are at most sixteen groups, of at most sixteen shares each.
    static constexpr size_t index_bits = 4;
    static constexpr size_t count_maximum = power2(index_bits);

    /// The share metadata and checksum are fixed length, and the share value
    /// is zero padded to a multiple of the word size.
    static constexpr size_t metadata_words = 4;
    static constexpr size_t checksum_words = shamir::rs1024::checksum_words;
    static constexpr size_t word_bits = shamir::rs1024::word_bits;
    static constexpr size_t fixed_words = metadata_words + checksum_words;

    /// Supports 20 to 59 word shares (128 to 512 bits).
    static constexpr size_t word_minimum = fixed_words +
        ceilinged_divide(to_bits(entropy_minimum), word_bits);
    static constexpr size_t word_maximum = fixed_words +
        ceilinged_divide(to_bits(entropy_maximum), word_bits);

    /// Valid master secret sizes (16 to 64 bytes, even).
    static bool is_valid_entropy_size(size_t size) NOEXCEPT;

    /// Valid share word counts (the word counts of valid secret sizes).
    static bool is_valid_word_count(size_t count) NOEXCEPT;

    /// Valid dictionary (en).
    static bool is_valid_dictionary(language identifier) NOEXCEPT;

    /// Valid passphrase (printable ascii).
    static bool is_valid_passphrase(const std::string& passphrase) NOEXCEPT;

    /// Valid group threshold and group member thresholds and counts.
    static bool is_valid_grouping(uint8_t group_threshold,
        const groups& groups) NOEXCEPT;

    /// The random bytes required to split a secret into the given groups.
    static size_t entropy_size(size_t secret_size, size_t group_threshold,
        const groups& groups) NOEXCEPT;

    /// Split a master secret into groups of shares, of which group_threshold
    /// groups are required to recover it. Entropy must be entropy_size bytes.
    /// Returns empty if any parameter is invalid.
    static list to_shares(const data_chunk& secret, const groups& groups,
        uint8_t group_threshold, const data_chunk& entropy,
        uint16_t identifier, bool extendable, uint8_t exponent,
        const std::string& passphrase="") NOEXCEPT;

    /// Recover the master secret from a threshold of groups of shares.
    /// Returns empty if the shares are inconsistent or insufficient, if a
    /// share group fails its digest, or if the passphrase is invalid.
    static data_chunk to_secret(const list& shares,
        const std::string& passphrase="") NOEXCEPT;

    /// Derive the "account private key" from a threshold of shares.
    /// This is also known as the wallet "root key" or "master private key".
    static hd_private to_key(const list& shares,
        const std::string& passphrase="",
        const context& context=btc_mainnet) NOEXCEPT;

    slip39() NOEXCEPT;

    /// Construct from a share mnemonic (sentence or words).
    /// Validity should be checked after construction.
    slip39(const std::string& sentence) NOEXCEPT;
    slip39(const string_list& words) NOEXCEPT;

    /// The identifier of the share set that the share belongs to.
    uint16_t identifier() const NOEXCEPT;

    /// True if the share set is an extendable backup.
    bool extendable() const NOEXCEPT;

    /// The exponent of the master secret encryption iteration count.
    uint8_t exponent() const NOEXCEPT;

    /// The group that the share belongs to, and the group threshold and
    /// count of its share set.
    uint8_t group_index() const NOEXCEPT;
    uint8_t group_threshold() const NOEXCEPT;
    uint8_t group_count() const NOEXCEPT;

    /// The share within its group, and the threshold of its group.
    uint8_t member_index() const NOEXCEPT;
    uint8_t member_threshold() const NOEXCEPT;

protected:
    /// The share metadata, which precedes the share value.
    struct fields
    {
        uint16_t identifier;
        bool extendable;
        uint8_t exponent;
        uint8_t group_index;
        uint8_t group_threshold;
        uint8_t group_count;
        uint8_t member_index;
        uint8_t member_threshold;
    };

    slip39(const data_chunk& value, const string_list& words,
        const fields& fields) NOEXCEPT;

    /// Split and recover at the secret size of the algorithm.
    template <typename Algorithm>
    static bool share(list& out, const data_chunk& secret,
        const groups& groups, uint8_t group_threshold,
        const data_chunk& entropy, uint16_t identifier, bool extendable,
        uint8_t exponent, const std::string& passphrase) NOEXCEPT;

    template <typename Algorithm>
    static bool combine(data_chunk& out, const list& shares,
        const std::string& passphrase) NOEXCEPT;

    static slip39 from_words(const string_list& words) NOEXCEPT;
    static string_list encoder(const data_chunk& value,
        const fields& fields) NOEXCEPT;
    static bool decoder(data_chunk& value, fields& out,
        const string_list& words) NOEXCEPT;

private:
    // The slip39 dictionary, from <dictionaries/slip39.cpp>.
    static const dictionaries dictionaries_;

    fields fields_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
