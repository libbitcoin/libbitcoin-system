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
#ifndef LIBBITCOIN_SYSTEM_BASE_2048_HPP
#define LIBBITCOIN_SYSTEM_BASE_2048_HPP

#include <string>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {

typedef boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<11, 11,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>> uint11_t;
typedef std::vector<uint11_t> base2048_chunk;

// This is the encoding used by both BIP39 and Electrum mnemonics.
// Dictionaries are unicode lower case and nfkd normalized.
// Words must be space (0x20) delimited and match dictionary (bytewise).
// Decoding returns false if specified language is 'none' and words are not in
// a single dictionary. Uses en decoding if 'none' speficied and all words in
// both en and fr. No other dictionaries have overlapping words with mismatched
// mappings.

/**
* Convert bytes to a base2048 sentence in the specified language.
* Returns empty if the language is not contained in the dictionary.
*/
BC_API std::string encode_base2048(const data_chunk& data,
    wallet::language language=wallet::language::en);

/**
 * Convert a base2048 sentence to bytes.
 * False if any word is not from the specified dictionary.
 */
BC_API bool decode_base2048(data_chunk& out, const std::string& in,
    wallet::language language=wallet::language::none);

/**
* Convert bytes to a base2048 word list in the specified language.
* Returns empty if the language is not contained in the dictionary.
*/
BC_API string_list encode_base2048_list(const data_chunk& data,
    wallet::language language=wallet::language::en);

/**
 * Convert a base2048 word list to bytes.
 * False if any word is not from the specified dictionary.
 */
BC_API bool decode_base2048_list(data_chunk& out, const string_list& in,
    wallet::language language=wallet::language::none);

// TODO: en.cppreference.com/w/cpp/language/user_literal

} // namespace system
} // namespace libbitcoin

#endif
