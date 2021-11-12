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
    /// Rules start at satoshi v0.3.0.
    /// Only satoshi-released hard forks, and enforced soft forks are included.
    no_rules = 0,

    // TODO:
    /// Promote nop1..nop10 [0.3.6] (hard fork, expansion).
    /// Combined script max 20,000 bytes, push_data 520 (soft fork, arbitrary).
    /// Initial script and push_data size limits (soft fork, arbitrary).
    /// Demote return, op_ver, verif, vernotif (soft fork, security).
    nops_rule = bit_right<uint32_t>(16),

    // TODO:
    /// Splits script evaluation, activated by [0.3.7] (hard fork, security).
    /// Per script max 10,000 bytes, one more total byte than prior, as the
    /// previous limit was imposed after concatenating with op_codeseparator.
    split_rule = bit_right<uint32_t>(17),

    // TODO:
    /// Invalidate op_cat, etc. [0.3.9] (soft fork, security).
    cats_rule = bit_right<uint32_t>(18),

    // TODO:
    /// The original rule leaked output value, was plugged here (soft fork).
    /// This was activated at block 74,638 [0.3.10], later to all blocks.
    plug_rule = bit_right<uint32_t>(19),

    // TODO:
    /// Limit block size to 1,000,000 bytes (soft fork, system DoS).
    /// Limit signature operations per block (soft fork, system DoS).
    /// This was activated at block 79,400 [0.3.12], and later to all blocks.
    size_rule = bit_right<uint32_t>(29),

    // TODO:
    /// Release [0.8.0] removal of hash limit, chain split at block 225,430.
    /// Original rule was from Berkely DB [unknown] (hard fork, determinism).
    unlock_rule = bit_right<uint32_t>(21),

    // TODO:
    /// This allowed double spends, applied and removed (hard fork, inflation).
    /// Activated (hard fork) in [0.15.0] deactivated (soft fork) in [0.16.3].
    inflation_rule = bit_right<uint32_t>(22),

    /// Pay-to-script-hash enabled (soft fork, feature).
    bip16_rule = bit_right<uint32_t>(1),

    /// No duplicated unspent transaction ids (soft fork, deflation).
    bip30_rule = bit_right<uint32_t>(2),

    /// Coinbase must include height, redundant w/bip30 (soft fork, arbitrary).
    bip34_rule = bit_right<uint32_t>(3),

    /// Finite monetary supply, effective at 13,440,000 (soft fork, inflation).
    bip42_rule = bit_right<uint32_t>(4),

    // TODO:
    /// Tx and input hashes max 4,500 per block (hard fork, determinism).
    /// This (initially) soft fork rule expires, which makes it a hard fork.
    bip50_rule = bit_right<uint32_t>(15),

    /// Strict DER signatures defined and required (soft fork, determinism).
    bip66_rule = bit_right<uint32_t>(5),

    /// Operation nop2 becomes check locktime verify (soft fork, feature).
    bip65_rule = bit_right<uint32_t>(6),

    /// Hard code bip34-based activation heights (hard fork, arbitrary).
    bip90_rule = bit_right<uint32_t>(7),

    /// Enforce relative locktime (soft fork, feature).
    bip68_rule = bit_right<uint32_t>(8),

    /// Operation nop3 becomes check sequence verify (soft fork, feature).
    bip112_rule = bit_right<uint32_t>(9),

    /// Use median time past for locktime (soft fork, security).
    bip113_rule = bit_right<uint32_t>(10),

    // TODO:
    /// Reduces threshold segregated witness signaling (soft fork, feature).
    bip91_rule = bit_right<uint32_t>(11),

    /// Segregated witness consensus layer (soft fork, feature).
    bip141_rule = bit_right<uint32_t>(12),

    /// Segregated witness v0 verification (soft fork, feature).
    bip143_rule = bit_right<uint32_t>(13),

    /// Segregated witness, dummy value malleability (soft fork, security).
    bip147_rule = bit_right<uint32_t>(14),

    /// TODO: taproot rules.

    // ************************************************************************
    // Future bitcoin forks work forward from bit 1.
    // Splitcoin/altcoin forks work backwards from bit 31.
    // ************************************************************************

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

    /// TODO:
    /// Rules that use BIP9 bit 0 first time activation.
    bip9_bit0_group =
        rule_fork::bip68_rule |
        rule_fork::bip112_rule |
        rule_fork::bip113_rule,

    /// TODO:
    /// Rules that use BIP9 bit 1 first time activation.
    bip9_bit1_group =
        rule_fork::bip141_rule |
        rule_fork::bip143_rule |
        rule_fork::bip147_rule,

    /// TODO:
    /// Rules that use BIP9 bit 4 first time activation.
    bip9_bit4_group =
        rule_fork::bip91_rule,

    /// TODO: rules that use taproot-style activation.

    /// Mask to set all rule bits.
    all_rules = bit_all<uint32_t>()
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
