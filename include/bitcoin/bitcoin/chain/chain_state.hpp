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
    typedef std::vector<uint8_t> versions;

    chain_state(bool testnet, const config::checkpoint::list& checkpoints);

    /// Properties.
    size_t next_height() const;
    uint32_t enabled_forks() const;
    uint32_t minimum_version() const;
    uint32_t median_time_past() const;
    uint32_t work_required() const;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // TODO: must set work_required_ and median_time_past_ from this call.
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /// Initialize enabled forks and minimum version for the given context.
    void set_context(size_t height, const versions& history);

    /// Determine it the flag is set in the active_forks member.
    bool is_enabled(rule_fork flag) const;

    /// Determine it the flag is set and enabled for the given block's version.
    bool is_enabled(const header& header, rule_fork flag) const;

    /// Determine if the block fails a checkpoint at next_height.
    bool is_checkpoint_failure(const header& header) const;

    /// Use to initialize the history collection and parameterize the query.
    const uint32_t sample_size;

private:
    size_t next_height_;
    uint32_t work_required_;
    uint32_t median_time_past_;
    uint32_t minimum_version_;
    uint32_t enabled_forks_;

    const bool testnet_;
    const size_t active_;
    const size_t enforce_;
    const size_t bip30_exception_height1_;
    const size_t bip30_exception_height2_;
    const size_t bip16_activation_height_;
    const config::checkpoint::list& checkpoints_;
};

} // namespace chain
} // namespace libbitcoin

#endif
