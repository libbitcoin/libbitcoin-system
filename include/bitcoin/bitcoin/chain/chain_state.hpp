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
#ifndef LIBBITCOIN_CHAIN_CHAIN_STATE_HPP
#define LIBBITCOIN_CHAIN_CHAIN_STATE_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

class BC_API chain_state
{
public:
    chain_state(const config::checkpoint::list& checkpoints);

    /// Determine it the flag is set in the active_forks member.
    bool is_enabled(rule_fork flag) const;

    /// Determine it the flag is set and enabled for the given block's version.
    bool is_enabled(const header& header, rule_fork flag) const;

    /// Determine if the block fails a checkpoint at next_height.
    bool is_checkpoint_failure(const header& header) const;

    size_t next_height;
    uint32_t active_forks;
    uint32_t median_time_past;
    uint32_t minimum_version;
    uint32_t work_required;

private:
    const config::checkpoint::list& checkpoints_;
};

} // namespace chain
} // namespace libbitcoin

#endif
