/**
 * Copyright (c) 2011-2032 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_RULE_FORK_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_RULE_FORK_HPP

#include <cstdint>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

enum rule_fork : uint32_t
{
    no_rules = 0,

    /// Pay-to-script-hash enabled (soft fork, feature).
    bip16_rule = bit_right<uint32_t>(1),

    /// No duplicated unspent transaction ids (soft fork, security).
    bip30_rule = bit_right<uint32_t>(2),

    /// Coinbase must include height (soft fork, security).
    bip34_rule = bit_right<uint32_t>(3),

    /// Finite monetary supply (soft fork, feature).
    bip42_rule = bit_right<uint32_t>(4),

    /// Hashes limited by bitcoind database locks (soft & hard fork, patch).
    bip50_rule = bit_right<uint32_t>(5),

    /// Strict DER signatures required (soft fork, security).
    bip66_rule = bit_right<uint32_t>(6),

    /// Operation nop2 becomes check locktime verify (soft fork, feature).
    bip65_rule = bit_right<uint32_t>(7),

    /// Hard code bip34-based activation heights (hard fork, optimization).
    bip90_rule = bit_right<uint32_t>(8),

    /// Enforce relative locktime (soft fork, feature).
    bip68_rule = bit_right<uint32_t>(9),

    /// Operation nop3 becomes check sequence verify (soft fork, feature).
    bip112_rule = bit_right<uint32_t>(10),

    /// Use median time past for locktime (soft fork, feature).
    bip113_rule = bit_right<uint32_t>(11),

    /// Segregated witness consensus layer (soft fork, feature).
    bip141_rule = bit_right<uint32_t>(12),

    /// Segregated witness v0 verification (soft fork, feature).
    bip143_rule = bit_right<uint32_t>(13),

    /// Prevent dummy value malleability (soft fork, feature).
    bip147_rule = bit_right<uint32_t>(14),

    // Future bitcoin forks work forward from << 15.
    // Splitcoin/altcoin forks work backwards from << 31.

    /// Fix Satoshi's time warp bug (hard fork, security).
    time_warp_patch = bit_right<uint32_t>(27),

    /// Fix target overflow for very low difficulty (hard fork, security).
    retarget_overflow_patch = bit_right<uint32_t>(28),

    /// Use scrypt hashing for proof of work (hard fork, feature).
    scrypt_proof_of_work = bit_right<uint32_t>(29),

    /// Perform difficulty retargeting (hard fork, regtest).
    retarget = bit_right<uint32_t>(30),

    /// Allow minimum difficulty blocks (hard fork, testnet).
    difficult = bit_right<uint32_t>(31),

    /// Rules that use bip34-based activation.
    bip34_activations =
        rule_fork::bip34_rule |
        rule_fork::bip65_rule |
        rule_fork::bip66_rule,

    /// Rules that use BIP9 bit zero first time activation.
    bip9_bit0_group =
        rule_fork::bip68_rule |
        rule_fork::bip112_rule |
        rule_fork::bip113_rule,

    /// Rules that use BIP9 bit one first time activation.
    bip9_bit1_group =
        rule_fork::bip141_rule |
        rule_fork::bip143_rule |
        rule_fork::bip147_rule,

    /// TODO: rules that use taproot-style activation.

    /// Mask to set all rule bits.
    all_rules = bit_all<uint32_t>()
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
