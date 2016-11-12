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
#ifndef LIBBITCOIN_SCRIPT_RULE_FORK_HPP
#define LIBBITCOIN_SCRIPT_RULE_FORK_HPP

#include <cstdint>

namespace libbitcoin {
namespace chain {

enum rule_fork : uint32_t
{
    no_rules = 0,

    /// pay-to-script-hash enabled
    bip16_rule = 1 << 0,

    /// no duplicated unspent transaction ids
    bip30_rule = 1 << 1,

    /// coinbase must include height
    bip34_rule = 1 << 2,

    /// strict DER signatures required
    bip66_rule = 1 << 3,

    /// nop2 becomes check locktime verify
    bip65_rule = 1 << 4,

    all_rules = 0xffffffff
};

} // namespace chain
} // namespace libbitcoin

#endif
