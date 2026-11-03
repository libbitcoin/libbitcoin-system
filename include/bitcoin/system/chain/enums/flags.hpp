/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

    // Coinbases from before bip34 activation that satisfy bip34 at a later
    // height, by duplicable height (original height), below 10,000,000:
    // 1,983,702 (164,384), 3,708,179 (169,895), 3,709,183 (170,307),
    // 3,712,990 (171,896), 3,713,413 (172,069), 3,714,082 (172,357),
    // 3,714,265 (172,428), 3,761,471 (183,669), 4,275,806 (196,988),
    // 5,208,854 (174,151), 5,327,833 (201,577), 7,299,941 (206,039),
    // 7,299,941 (206,354), 8,624,845 (147,396), 8,631,390 (149,732),
    // 8,631,629 (149,813), 8,631,693 (149,838), 8,632,995 (150,283),
    // 8,636,474 (151,491), 8,638,716 (152,374), 8,639,231 (152,599),
    // 8,641,929 (153,662), 9,558,101 (177,628).
    // delvingbitcoin.org/t/great-consensus-cleanup-revival/710/4
    // gist.github.com/darosior/3a5ac0a8d935fa9d3e90310590ca6699

    /// No duplicated unspent transaction ids (soft fork, deflation).
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

    /// No retarget block more than two hours before its parent (soft fork, security).
    bip54_timewarp_rule = bit_right<uint32_t>(17),

    // bitcoind consensus changes, not specified before release.
    // ------------------------------------------------------------------------

    // [0.8.0] The LevelDB store drops the Berkeley DB lock limit, chain split
    // at block 225,430 (hard fork, determinism).
    // github.com/bitcoin/bitcoin/pull/1677

    // [0.8.1] Max 4,500 tx and input hashes per block before 2013-05-15 (soft
    // fork, determinism), then expired (hard fork, determinism). Specified
    // after the fact as bip50, code removed in [0.9.0].
    // github.com/bitcoin/bitcoin/commit/8bd02881899bbae2d8e5082081e02c7d577994e5
    // github.com/bitcoin/bitcoin/pull/2657

    // [0.12.0] No bip30 check above bip34 activation at block 227,931 (hard
    // fork, deflation).
    // github.com/bitcoin/bitcoin/pull/6931

    // [0.15.0] The inflation bug, duplicate inputs accepted in blocks (hard
    // fork, inflation). The check was removed from blocks in [0.14.0].
    // github.com/bitcoin/bitcoin/pull/9049
    // github.com/bitcoin/bitcoin/pull/10195

    // [0.16.0] bip16 moved from its activation time to block 173,805 (hard
    // fork, arbitrary).
    // github.com/bitcoin/bitcoin/pull/11389

    // [0.16.3] The inflation bug fixed (soft fork, inflation).
    // github.com/bitcoin/bitcoin/pull/14247

    // [0.17.0] bip16 and bip141 script rules from genesis, one bip16 block
    // excepted by hash (soft fork, arbitrary).
    // github.com/bitcoin/bitcoin/pull/11739

    // [0.17.0] bip30 checked again from block 1,983,702 (soft fork,
    // deflation).
    // github.com/bitcoin/bitcoin/pull/12204

    // [0.19.0] bip9 bit0 group buried at block 419,328 and bit1 group at
    // block 481,824 (hard fork, arbitrary).
    // github.com/bitcoin/bitcoin/pull/16060

    // [24.0] bip341 and bip342 script rules wherever bip141 applies, so from
    // genesis, one block excepted by hash (soft fork, arbitrary).
    // github.com/bitcoin/bitcoin/pull/23536

    // [30.0] Checkpoints removed, forks below the last checkpoint accepted
    // (hard fork, security).
    // github.com/bitcoin/bitcoin/pull/31649

    // bip91 was implemented, and proposed for [0.14.3] on 2017-07-21, but was
    // never merged or released.
    // github.com/bitcoin/bitcoin/pull/10444
    // github.com/bitcoin/bitcoin/pull/10900

    /// Signet only
    signet_rule = bit_right<uint32_t>(26),

    /// Testnet4 only
    time_warp_patch = bit_right<uint32_t>(27),

    /// Testnet4 only
    block_storm_patch = bit_right<uint32_t>(28),

    /// Litecoin rules.
    /// -----------------------------------------------------------------------

    /// Fix Satoshi's time warp bug (hard fork, security).
    ltc_time_warp_patch = bit_right<uint32_t>(29),

    /// Fix target overflow for very low difficulty (hard fork, security).
    ltc_retarget_overflow_patch = bit_right<uint32_t>(30),

    /// Use scrypt hashing for proof of work (hard fork, feature).
    ltc_scrypt_proof_of_work = bit_right<uint32_t>(31),

    /// Aggregates
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

    /// Mask to set all rule bits (32).
    all_rules = bit_all<uint32_t>
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
