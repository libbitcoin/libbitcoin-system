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
#include <map>
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
 * to static memory no run-time overhead.
 */
typedef std::array<const char*, wordlist_size> wordlist;

/**
 * Dictionary languages.
 */
enum class language
{
    en,
    es,
    ja,
    zh_Hans,
    zh_Hant,
    unknown
};

/**
 * Multilingual word dictionary type.
 * It references the wordlists by pointer,
 * allowing them to live in static memory.
 */
typedef std::map<language, const wordlist*> dictionary_type;

/**
 * Multilingual word dictionary.
 */
extern const dictionary_type dictionary;

} // namespace bip39
} // namespace libbitcoin

#endif
