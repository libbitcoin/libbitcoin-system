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

    /// allow minimum difficulty blocks (hard fork, testnet)
    easy_blocks = 1 << 0,

    /// pay-to-script-hash enabled (soft fork)
    bip16_rule = 1 << 1,

    /// no duplicated unspent transaction ids (hard fork, necessary)
    bip30_rule = 1 << 2,

    /// coinbase must include height (soft fork)
    bip34_rule = 1 << 3,

    /// strict DER signatures required (soft fork)
    bip66_rule = 1 << 4,

    /// nop2 becomes check locktime verify (soft fork)
    bip65_rule = 1 << 5,

    /// assume hash collisions cannot happen (hard fork, invalid)
    allowed_duplicates = 1 << 6,

    /// hard code activation heights (hard fork, unnecessary)
    deep_freeze = 1 << 7,

    /// rules that require bip34 style activation
    activations =
        rule_fork::bip34_rule |
        rule_fork::bip65_rule |
        rule_fork::bip66_rule,

    /// the valid set of changes to the original rule set
    consensus =
        rule_fork::bip16_rule |
        rule_fork::bip30_rule |
        rule_fork::bip34_rule |
        rule_fork::bip65_rule |
        rule_fork::bip66_rule,

    all_rules = 0xffffffff
};

} // namespace machine
} // namespace libbitcoin

#endif
