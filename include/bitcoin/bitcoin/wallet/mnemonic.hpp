/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MNEMONIC_HPP
#define LIBBITCOIN_MNEMONIC_HPP

#include <string>
#include <vector>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/dictionary.hpp>

namespace libbitcoin {
namespace bip39 {

/**
 * A valid mnemonic word count is evenly divisible by this number.
 */
BC_CONSTEXPR size_t word_multiple = 3;

/**
 * A valid seed byte count is evenly divisible by this number.
 */
BC_CONSTEXPR size_t seed_multiple = 4;

/**
 * A valid mnemonic has at least this many words.
 */
BC_CONSTEXPR size_t min_word_count = 12;

/**
 * A valid mnemonic has no more than this many words.
 */
BC_CONSTEXPR size_t max_word_count = 128;

/**
 * Create a new mnenomic (list of words) from provided entropy and a dictionary
 * selection. The mnemonic can later be converted to a seed for use in wallet
 * creation. Entropy byte count must be evenly divisible by 3.
 */
BC_API string_list create_mnemonic(data_slice entropy, 
    bip39::language language=bip39::language::en);

/**
 * Convert a mnemonic and optional passphrase to a seed for use in seeding
 * wallet creation. The words must have been created using mnemonic encoding.
 * Any passphrase can be used and will change the resulting seed.
 */
BC_API data_chunk decode_mnemonic(const string_list& mnemonic,
    const std::string& passphrase="");

} // namespace bip39
} // namespace libbitcoin

#endif
