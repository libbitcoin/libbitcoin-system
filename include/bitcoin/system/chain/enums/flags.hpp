/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_FLAGS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_FLAGS_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

enum flags : uint32_t
{
    /// Rules start at satoshi v0.3.0.
    /// Only satoshi-released hard forks, and enforced soft forks are included.
    no_rules = 0,

    /// Disallow minimum difficulty blocks (hard fork, !testnet).
    difficult = bit_right<uint32_t>(0),

    /// Perform difficulty retargeting (hard fork, !regtest).
    retarget = bit_right<uint32_t>(1),

    /// Pay-to-script-hash enabled (soft fork, feature).
    bip16_rule = bit_right<uint32_t>(2),

    /// No duplicated unspent transaction ids (soft/hard/soft fork, deflation).
    /// This (initially) soft fork rule expires and then unexpires (hard/soft).
    bip30_rule = bit_right<uint32_t>(3),

    /// Coinbase must include height, redundant w/bip30 (soft fork, arbitrary).
    bip34_rule = bit_right<uint32_t>(4),

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

    /// Segregated witness consensus layer (soft fork, feature).
    bip141_rule = bit_right<uint32_t>(11),

    /// Segregated witness v0 verification (soft fork, feature).
    bip143_rule = bit_right<uint32_t>(12),

    /// Segregated witness, dummy value malleability (soft fork, security).
    bip147_rule = bit_right<uint32_t>(13),

    /// Finite monetary supply, effective at 13,440,000 (soft fork, inflation).
    bip42_rule = bit_right<uint32_t>(14),

    /// Taproot: witness version 1 spending rules.
    bip341_rule = bit_right<uint32_t>(15),

    /// Taproot: validation of tapscripts.
    bip342_rule = bit_right<uint32_t>(16),

    /// Litecoin rules.
    /// -----------------------------------------------------------------------

    /// Fix Satoshi's time warp bug (hard fork, security).
    time_warp_patch = bit_right<uint32_t>(17),

    /// Fix target overflow for very low difficulty (hard fork, security).
    retarget_overflow_patch = bit_right<uint32_t>(18),

    /// Use scrypt hashing for proof of work (hard fork, feature).
    scrypt_proof_of_work = bit_right<uint32_t>(19),

    /// TODO: hardwired or obsoleted (historical).
    /// -----------------------------------------------------------------------

    /// Promote nop1..nop10 [0.3.6] (hard fork, expansion).
    /// Combined script max 20,000 bytes, push_data 520 (soft fork, arbitrary).
    /// Initial script and push_data size limits (soft fork, arbitrary).
    /// Demote return, op_ver, verif, vernotif (soft fork, security).
    nops_rule = bit_right<uint32_t>(20),

    /// Splits script evaluation, activated by [0.3.7] (hard fork, security).
    /// Per script max 10,000 bytes, one more total byte than prior, as the
    /// previous limit was imposed after concatenating with op_codeseparator.
    split_rule = bit_right<uint32_t>(21),

    /// Invalidate op_cat, etc. [0.3.9] (soft fork, security).
    cats_rule = bit_right<uint32_t>(22),

    /// The original rule leaked output value, was plugged here (soft fork).
    /// This was activated at block 74,638 [0.3.10], later to all blocks.
    plug_rule = bit_right<uint32_t>(23),

    /// Limit block size to 1,000,000 bytes (soft fork, system DoS).
    /// Limit signature operations per block (soft fork, system DoS).
    /// This was activated at block 79,400 [0.3.12], and later to all blocks.
    size_rule = bit_right<uint32_t>(24),

    /// Release [0.8.0] removal of hash limit, chain split at block 225,430.
    /// Original rule was from Berkely DB [unknown] (hard fork, determinism).
    unlock_rule = bit_right<uint32_t>(23),

    /// This allowed double spends, applied and removed (hard fork, inflation).
    /// Activated (hard fork) in [0.15.0] deactivated (soft fork) in [0.16.3].
    inflation_rule = bit_right<uint32_t>(25),

    /// Tx and input hashes max 4,500 per block (soft/hard fork, determinism).
    /// This (initially) soft fork rule expires, which makes it a hard fork.
    bip50_rule = bit_right<uint32_t>(26),

    /// Reduces threshold segregated witness signaling (soft fork, feature).
    bip91_rule = bit_right<uint32_t>(27),

    /// Agregates
    /// -----------------------------------------------------------------------

    /// Rules that use bip34-based activation.
    bip34_activations =
        flags::bip34_rule |
        flags::bip65_rule |
        flags::bip66_rule,

    /// Rules that use BIP9 bit 0 ("versionbits") first time activation.
    bip9_bit0_group =
        flags::bip68_rule |
        flags::bip112_rule |
        flags::bip113_rule,

    /// Rules that use BIP9 bit 1 ("segwit") first time activation.
    bip9_bit1_group =
        flags::bip141_rule |
        flags::bip143_rule |
        flags::bip147_rule,

    /// Rules that use BIP9 bit 2 ("taproot") first time activation.
    bip9_bit2_group =
        flags::bip341_rule |
        flags::bip342_rule,

    /////// Rules that use BIP9 bit 4 ("segsignal") first time activation.
    ////bip9_bit4_group =
    ////    flags::bip91_rule,

    /// Mask to set all rule bits (32).
    all_rules = bit_all<uint32_t>
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
