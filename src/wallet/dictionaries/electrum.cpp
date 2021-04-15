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
#include <bitcoin/system/wallet/electrum.hpp>

#include <bitcoin/system/wallet/mnemonic.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// TODO: consider adding all BIP39 word lists to Electrum.
// The portuguese wordlist at the site site below is Electrum v1.
// github.com/spesmilo/electrum/tree/master/electrum/wordlist

// Electrum v2 word lists are all identical to mnemonic (BIP39) word lists.
const electrum::dictionary::words& electrum::en = mnemonic::en;
const electrum::dictionary::words& electrum::es = mnemonic::es;
const electrum::dictionary::words& electrum::ja = mnemonic::ja;
const electrum::dictionary::words& electrum::zh_Hans = mnemonic::zh_Hans;

} // namespace wallet
} // namespace system
} // namespace libbitcoin
