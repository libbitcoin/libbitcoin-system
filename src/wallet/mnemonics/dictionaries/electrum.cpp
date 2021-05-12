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
#include <bitcoin/system/wallet/mnemonics/electrum.hpp>

#include <bitcoin/system/wallet/mnemonics/mnemonic.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// github.com/spesmilo/electrum/tree/master/electrum/wordlist
// The portuguese wordlist in the Electrum v1 repo is for Electrum v2.
// The Electrum v1 repo includes en, es, ja, and zh_Hant only.
// This list has been extended to include all BIP39 word lists.

// Electrum v2 word lists are currently identical to BIP39 word lists.
const electrum::dictionary::words& electrum::en = mnemonic::en;
const electrum::dictionary::words& electrum::es = mnemonic::es;
const electrum::dictionary::words& electrum::it = mnemonic::it;
const electrum::dictionary::words& electrum::fr = mnemonic::fr;
const electrum::dictionary::words& electrum::cs = mnemonic::cs;
const electrum::dictionary::words& electrum::pt = mnemonic::pt;
const electrum::dictionary::words& electrum::ja = mnemonic::ja;
const electrum::dictionary::words& electrum::ko = mnemonic::ko;
const electrum::dictionary::words& electrum::zh_Hans = mnemonic::zh_Hans;
const electrum::dictionary::words& electrum::zh_Hant = mnemonic::zh_Hant;

} // namespace wallet
} // namespace system
} // namespace libbitcoin
