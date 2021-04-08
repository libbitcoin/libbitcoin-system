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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_DICTIONARY_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_DICTIONARY_HPP

#include <array>
#include <vector>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// An Electrum v1 mnemonic dictionary has exactly this many words.
 static BC_CONSTEXPR size_t dictionary_size_v1 = 1626;

// Dictionary for creating "old" Electrum mnemonics.
typedef std::array<const char*, dictionary_size_v1> dictionary_v1;

namespace language {
namespace electrum {

extern const dictionary& en;
extern const dictionary& es;
extern const dictionary& ja;
extern const dictionary& zh_Hans;

// Word lists (excluding en_v1) from:
// github.com/spesmilo/electrum/tree/master/electrum/wordlist
extern const dictionary_list all;

// Electrum v1 dictionary required for v2 compatibility.
extern const dictionary_v1 en_v1;

} // namespace electrum
} // namespace language
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
