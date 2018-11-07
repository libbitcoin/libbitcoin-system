/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_WALLET_ELECTRUM_HPP
#define LIBBITCOIN_WALLET_ELECTRUM_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/wallet/dictionary.hpp>

namespace libbitcoin {
namespace wallet {
namespace electrum {

/**
 * We're using a checked arbitrary precision integer to more closely
 * match the electrum implementation in terms of compatibility, that
 * uses Python Long integers which have unlimited precision.
 *
 * From boost documentation regarding the sign type: Determines
 * whether the resulting type is signed or not. Note that for
 * arbitrary precision types this parameter must be
 * signed_magnitude. For fixed precision types then this type may be
 * either signed_magnitude or unsigned_magnitude.
 *
 * Usage in the reference electrum implementation shows that the
 * values used must be >= 1, which means an unsigned variant would be
 * ideal.  Since it's created from a base16 encoded string form of
 * entropy internally, it should not be possible to have a negative
 * value.
 */
typedef boost::multiprecision::checked_cpp_int cpp_int;

/**
 * Valid seed types.
 */
enum class seed : uint8_t
{
    standard = 1,
    witness = 2,
    two_factor_authentication = 3
};

/**
 * Valid seed prefixes.
 */
static const data_chunk seed_prefix_standard{ 0x01 };
static const data_chunk seed_prefix_witness{ 0x01, 0x00 };
static const data_chunk seed_prefix_two_factor_authentication{ 0x01, 0x01 };

/**
 * Represents an electrum word list.
 */
typedef string_list word_list;

#ifdef WITH_ICU

/**
 * Create a new mnenomic (list of words) from provided entropy, an
 * optional dictionary selection, and seed prefix, . The mnemonic can
 * later be converted to a seed for use in electrum wallet creation.
 */
BC_API word_list create_mnemonic(const data_chunk& entropy,
    const dictionary& lexicon=language::en,
    seed prefix=electrum::seed::standard);

/**
 * Checks an electrum mnemonic against a dictionary to determine if
 * the words are spelled correctly and the seed prefix matches.  The
 * words must have been created using electrum encoding.
 */
BC_API bool validate_mnemonic(const word_list& mnemonic,
    const dictionary& lexicon,
    seed prefix=electrum::seed::standard);

/**
 * Checks that a mnemonic is valid in at least one of the provided languages.
 */
BC_API bool validate_mnemonic(const word_list& mnemonic,
    const dictionary_list& lexicons=language::all,
    seed prefix=electrum::seed::standard);

/**
 * Convert an electrum mnemonic and passphrase to a wallet-generation
 * seed.  Any passphrase can be used and will change the resulting
 * seed.
 */
BC_API long_hash decode_mnemonic(const word_list& mnemonic,
    const std::string& passphrase);

#endif

/**
 * Convert an electrum mnemonic without a passphrase to a
 * wallet-generation seed.
 */
BC_API long_hash decode_mnemonic(const word_list& mnemonic);

} // namespace electrum
} // namespace wallet
} // namespace libbitcoin

#endif
