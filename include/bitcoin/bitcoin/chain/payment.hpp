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
#ifndef LIBBITCOIN_CHAIN_PAYMENT_HPP
#define LIBBITCOIN_CHAIN_PAYMENT_HPP

#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

enum class payment_type
{
    pubkey,             // Pubkey and checksig in output.
    pubkey_hash,        // Payment to hash of pubkey
    script_hash,        // Payment to multisig address
    stealth_info,       // Stealth information (meta-output)
    multisig,           // Multisig
    pubkey_sig,         // Pubkey input script.
    pubkey_hash_sig,    // Pubkey hash input script.
    multi_pubkey_sig,   // BIP11 input script code.
    script_code_sig,    // BIP16 input script code.
    non_standard
};

}
}

#endif
