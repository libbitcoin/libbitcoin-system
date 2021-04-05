/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_DICTIONARY_HPP
#define LIBBITCOIN_SYSTEM_WALLET_DICTIONARY_HPP

#include <array>
#include <functional>
#include <vector>
#include <bitcoin/system/compat.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// A valid mnemonic dictionary has exactly this many words.
// This size applies to both Electrum (>v1) and bip39 word lists.
static BC_CONSTEXPR size_t dictionary_size = 2048;

// Dictionary names.
enum class reference
{
    en,
    es,
    it,
    fr,
    cs,
    pt,
    ja,
    ko,
    zh_Hans,
    zh_Hant,
    none
};

// A dictionary (bip39 wordlist) for creating mnemonics.
// The compiler can write the char* array (POD type) directly to static memory.
typedef struct
{
    reference name;
    std::array<const char*, dictionary_size> words;
} dictionary;

// A collection of dictionaries.
typedef std::vector<std::reference_wrapper<const dictionary>> dictionary_list;

namespace language
{

// Individual bip39 languages:
extern const dictionary en;
extern const dictionary es;
extern const dictionary it;
extern const dictionary fr;
extern const dictionary cs;
extern const dictionary pt;
extern const dictionary ja;
extern const dictionary ko;
extern const dictionary zh_Hans;
extern const dictionary zh_Hant;

// A collection of all bip39 languages:
extern const dictionary_list all;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
