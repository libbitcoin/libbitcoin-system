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
#ifndef LIBBITCOIN_DICTIONARY_HPP
#define LIBBITCOIN_DICTIONARY_HPP

#include <array>
#include <vector>
#include <bitcoin/bitcoin/compat.hpp>

namespace libbitcoin {
namespace bip39 {

/**
 * A valid mnemonic wordlist has exactly this many words.
 */
BC_CONSTEXPR size_t wordlist_size = 2048;

/**
 * A "wordlist" for creating mnemonics, as defined in bip39.
 * This is a POD type, which means the compiler can write it directly
 * to static memory with no run-time overhead.
 */
typedef std::array<const char*, wordlist_size> wordlist;

/**
 * A collection of candidate wordlists for mnemonic validation.
 */
typedef std::vector<const wordlist*> wordlist_list;

namespace language
{
    // Individual built-in languages:
    extern const wordlist en;
    extern const wordlist es;
    extern const wordlist ja;
    extern const wordlist zh_Hans;
    extern const wordlist zh_Hant;

    // All built-in languages:
    extern const wordlist_list all;
}

} // namespace bip39
} // namespace libbitcoin

#endif
