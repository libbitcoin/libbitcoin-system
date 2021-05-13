/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/keys/witness_address.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/mnemonics/dictionaries.hpp>
#include <bitcoin/system/wallet/mnemonics/dictionary.hpp>
#include <bitcoin/system/wallet/mnemonics/electrum_v1.hpp>
#include <bitcoin/system/wallet/mnemonics/mnemonic.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A wallet mnemonic, as defined by the second Electrum implementation.
class BC_API electrum
  : public electrum_v1
{
public:
    /// Seed prefix types.
    /// The values for old, bip39 and none are not actual prefixes but 
    /// here for consistency when handling electrum exceptional conditions.
    enum class seed_prefix: uint8_t
    {
        old,
        bip39,
        standard,
        witness,
        two_factor_authentication,
        two_factor_authentication_witness,
        none
    };

    typedef wallet::dictionary<2048> dictionary;
    typedef wallet::dictionaries<10, dictionary::size()> dictionaries;

    /// Publish Electrum word lists (same as BIP39 word lists).
    static const dictionary::words& en;
    static const dictionary::words& es;
    static const dictionary::words& it;
    static const dictionary::words& fr;
    static const dictionary::words& cs;
    static const dictionary::words& pt;
    static const dictionary::words& ja;
    static const dictionary::words& ko;
    static const dictionary::words& zh_Hans;
    static const dictionary::words& zh_Hant;

    // Supports 132 to 506 bits of entropy (12 to 46 words).
    // Entropy byte limits are rounded up to 17 to 64 bytes.
    static constexpr size_t strength_minimum = 132;
    static constexpr size_t strength_maximum = 506;

    /// The dictionary, limited by identifier, that contains all words.
    /// If 'none' is specified all dictionaries are searched.
    /// If 'none' is specified and contained by both en and fr, returns en.
    static language contained_by(const string_list& words,
        language identifier=language::none);

    /// Valid dictionaries (en, es, it, fr, cs, pt, ja, ko, zh_Hans, zh_Hant).
    static bool is_valid_dictionary(language identifier);

    /// Valid entropy values (17 to 64 bytes).
    static bool is_valid_entropy_size(size_t size);
    
    /// Valid word counts (12 to 46 words).
    static bool is_valid_word_count(size_t count);

    /// Returns false if prefix is 'old', 'bip39' or 'none'.
    /// Returns true if the seed of the words has the given prefix.
    static bool is_version(const string_list& words, seed_prefix prefix);

    /// Obtain the enumerated prefix corresponding to the words.
    /// Returns 'old', 'bip39' or 'none' if not a valid electrum v2 seed.
    /// Otherwise words must be prenormalized if WITH_ICU is not defined.
    /// A prefix other than 'none' implies the words represent a valid seed.
    static seed_prefix to_prefix(const string_list& words);

    /// Obtain the version corresponding to the enumeration value.
    static std::string to_version(seed_prefix prefix);

    /// This instance is initialized invalid, but can be assigned to.
    electrum();

    /// The instance should be tested for validity after construction.
    electrum(const electrum& other);
    electrum(const std::string& sentence, language identifier=language::none);
    electrum(const string_list& words, language identifier=language::none);
    electrum(const data_chunk& entropy, seed_prefix prefix, language lexicon);

    /// The prefix indicates the intended use of the seed.
    seed_prefix prefix() const;

    /// The seed derived from mnemonic entropy and an optional passphrase.
    /// Returns invalid result with non-ascii passphrase and WITH_ICU undefined.
    hd_private to_seed(const std::string& passphrase="",
        uint64_t chain=hd_private::mainnet) const;

    /// Serialized sentence.
    friend std::istream& operator>>(std::istream& in, electrum& to);
    friend std::ostream& operator<<(std::ostream& out, const electrum& of);

protected:
    /// Map entropy to entropy bit count (132 to 506 bits).
    static size_t entropy_bits(const data_slice& entropy);
    
    /// Map words to entropy bit count (132 to 506 bits).
    static size_t entropy_bits(const string_list& words);

    /// Map entropy bits to sufficient entropy size (17 to 64 bytes).
    static size_t entropy_size(size_t bit_strength);

    /// Map words to entropy size (17 to 64 bytes).
    static size_t entropy_size(const string_list& words);
    
    /// Map entropy size to word count (12 to 46 words).
    static size_t word_count(const data_slice& entropy);

    /// Map entropy bits to sufficient word count (12 to 46 words).
    static size_t word_count(size_t bit_strength);

    /// Map entropy size to number of unused bits.
    static uint8_t unused_bits(const data_slice& entropy);

    /// Determine the number of unused bytes of entropy.
    static uint8_t unused_bytes(const data_slice& entropy);

    /// Map entropy size to usable bytes (unused bits may remain).
    static size_t usable_size(const data_slice& entropy);

    /// Constructors.
    electrum(const data_chunk& entropy, const string_list& words,
        language identifier, seed_prefix prefix);

    static bool is_valid_seed_prefix(seed_prefix prefix);
    static bool is_valid_two_factor_authentication_size(size_t count);

    typedef struct { data_chunk entropy; string_list words; } result;
    static result grinder(const data_chunk& entropy, seed_prefix prefix,
        language identifier, size_t limit);
    static string_list encoder(const data_chunk& entropy, language identifier);
    static data_chunk decoder(const string_list& words, language identifier);
    static hd_private seeder(const string_list& words,
        const std::string& passphrase, uint64_t chain);

    static seed_prefix prefixer(const string_list& words);
    static bool validator(const string_list& words, seed_prefix prefix);

    static electrum from_words(const string_list& words, language identifier);
    static electrum from_entropy(const data_chunk& entropy, seed_prefix prefix,
        language identifier);

private:
    // All Electrum dictionaries, subset of <dictionaries/mnemonic.cpp>.
    static const dictionaries dictionaries_;

    // Unique to Electrum v2.
    seed_prefix prefix_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
