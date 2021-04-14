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

// Electrum v2 word lists from:
// github.com/spesmilo/electrum/tree/master/electrum/wordlist
const electrum::dictionaries dictionaries_
{
    {
        electrum::dictionary{ language::en, electrum::en },
        electrum::dictionary{ language::es, electrum::es },
        electrum::dictionary{ language::ja, electrum::ja },
        electrum::dictionary{ language::zh_Hans, electrum::zh_Hans }
    }
};

// Electrum word lists are all references to mnemonic (BIP39) word lists.
const electrum::dictionary::words& en = mnemonic::en;
const electrum::dictionary::words& es = mnemonic::es;
const electrum::dictionary::words& ja = mnemonic::ja;
const electrum::dictionary::words& zh_Hans = mnemonic::zh_Hans;

} // namespace wallet
} // namespace system
} // namespace libbitcoin
