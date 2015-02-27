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

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

const uint16_t max_word_count = 128;
const uint16_t min_entropy_bits = 128;
const uint16_t max_digest_buf_len = 512;

enum class bip39_language {
    en, es, ja, zh
};

typedef std::vector<std::string> string_list;
typedef std::map<bip39_language, string_list> dictionary;

// seed to mnemonic
BC_API string_list encode_mnemonic(
    /* data_slice seed, const std::string &passphrase="", */
    data_slice seed, bip39_language language = bip39_language::en);

// mnemonic to seed
BC_API data_chunk decode_mnemonic(
    const string_list &words, const std::string &passphrase = "");

} // namespace libbitcoin

#endif
