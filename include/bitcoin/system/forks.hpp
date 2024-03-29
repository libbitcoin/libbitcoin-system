/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_FORKS_HPP
#define LIBBITCOIN_SYSTEM_FORKS_HPP

namespace libbitcoin {
namespace system {

/// Chain state accepts configured forks and maps them onto active flags
/// for the a given validaton context (i.e. block). These are exposed as
/// chain::chain_state.flags() and chain::context.flags.
typedef struct
{
    bool bip16;
    bool bip30;
    bool bip30_deactivate;
    bool bip30_reactivate;
    bool bip34;
    bool bip42;
    bool bip65;
    bool bip66;
    bool bip68;
    bool bip90;
    bool bip112;
    bool bip113;
    bool bip141;
    bool bip143;
    bool bip147;
    bool retarget;                // !regtest
    bool difficult;               // !testnet
    bool time_warp_patch;         // litecoin
    bool retarget_overflow_patch; // litecoin
    bool scrypt_proof_of_work;    // litecoin
} forks;

} // namespace system
} // namespace libbitcoin

#endif
