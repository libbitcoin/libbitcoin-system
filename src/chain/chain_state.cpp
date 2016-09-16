/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/chain_state.hpp>

#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::config;

chain_state::chain_state(const checkpoint::list& checkpoints)
  : checkpoints_(checkpoints)
{
}

bool chain_state::is_enabled(rule_fork flag) const
{
    return script::is_enabled(active_forks, flag);
}

bool chain_state::is_enabled(const header& header, rule_fork flag) const
{
    return (is_enabled(flag)) &&
       ((flag == rule_fork::bip65_rule && header.version >= bip65_version) ||
        (flag == rule_fork::bip66_rule && header.version >= bip66_version) ||
        (flag == rule_fork::bip34_rule && header.version >= bip34_version));
}

bool chain_state::is_checkpoint_failure(const header& header) const
{
    return !checkpoint::validate(header.hash(), next_height, checkpoints_);
}

} // namespace chain
} // namespace libbitcoin
