/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_MACHINE_RULE_FORK_HPP
#define LIBBITCOIN_MACHINE_RULE_FORK_HPP

#include <cstdint>

namespace libbitcoin {
namespace machine {

enum rule_fork : uint32_t
{
    no_rules = 0,

    /// Allow minimum difficulty blocks (hard fork, testnet).
    easy_blocks = 1u << 0,

    /// Pay-to-script-hash enabled (soft fork, feature).
    bip16_rule = 1u << 1,

    /// No duplicated unspent transaction ids (hard fork, security).
    bip30_rule = 1u << 2,

    /// Coinbase must include height (soft fork, security).
    bip34_rule = 1u << 3,

    /// Strict DER signatures required (soft fork, security).
    bip66_rule = 1u << 4,

    /// Operation nop2 becomes check locktime verify (soft fork, feature).
    bip65_rule = 1u << 5,

    /// Hard code bip34-based activation heights (hard fork, optimization).
    bip90_rule = 1u << 6,

    /// Assume hash collisions cannot happen (hard fork, optimization).
    allow_collisions = 1u << 7,

    /// Enforce relative locktime (soft fork, feature).
    bip68_rule = 1u << 8,

    /// Operation nop3 becomes check sequence verify (soft fork, feature).
    bip112_rule = 1u << 9,

    /// Use median time past for locktime (soft fork, feature).
    bip113_rule = 1u << 10,

    /// Sentinel bit to indicate tx has not been validated.
    unverified = 1u << 31,

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

    /// Simple mask to set all bits.
    all_rules = 0xffffffff
};

} // namespace machine
} // namespace libbitcoin

#endif
