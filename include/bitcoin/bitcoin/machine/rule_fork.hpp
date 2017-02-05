/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MACHINE_RULE_FORK_HPP
#define LIBBITCOIN_MACHINE_RULE_FORK_HPP

#include <cstdint>

namespace libbitcoin {
namespace machine {

enum rule_fork : uint32_t
{
    no_rules = 0,

    /// Allow minimum difficulty blocks (hard fork, testnet)
    easy_blocks = 1u << 0,

    /// Pay-to-script-hash enabled (soft fork)
    bip16_rule = 1u << 1,

    /// No duplicated unspent transaction ids (hard fork, security)
    bip30_rule = 1u << 2,

    /// Coinbase must include height (soft fork)
    bip34_rule = 1u << 3,

    /// Strict DER signatures required (soft fork)
    bip66_rule = 1u << 4,

    /// Operation nop2 becomes check locktime verify (soft fork)
    bip65_rule = 1u << 5,

    /// Hard code activation heights (hard fork, optimization)
    bip90_rule = 1u << 6,

    /// Assume hash collisions cannot happen (hard fork, optimization)
    allow_collisions = 1u << 7,

    /// Sentinel bit to indicate tx has not been validated.
    unverified = 1u << 31,

    /// Rules that require bip34 style activation
    activations =
        rule_fork::bip34_rule |
        rule_fork::bip65_rule |
        rule_fork::bip66_rule,

    /// Simple mask to set all bits.
    all_rules = 0xffffffff
};

} // namespace machine
} // namespace libbitcoin

#endif
