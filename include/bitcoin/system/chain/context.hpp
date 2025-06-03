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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_CONTEXT_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/policy.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API context final
{
public:
    /// Determine if the flag is active for this block.
    inline bool is_enabled(chain::flags flag) const NOEXCEPT
    {
        return to_bool(flag & flags);
    }

    // ************************************************************************
    // CONSENSUS: Soft forks imposed minimum block versioning using a signed
    // interpretation of header.version, which would otherwise be unsigned.
    // ************************************************************************
    inline bool is_insufficient_version(uint32_t version) const NOEXCEPT
    {
        return is_nonzero(minimum_block_version) &&
            to_signed(version) < to_signed(minimum_block_version);
    }

    inline bool is_anachronistic_timestamp(uint32_t time_stamp) const NOEXCEPT
    {
        return time_stamp <= median_time_past;
    }

    inline bool is_invalid_work(uint32_t bits) const NOEXCEPT
    {
        return bits != work_required;
    }

    /// Header context within chain.
    uint32_t flags;
    uint32_t timestamp;
    uint32_t median_time_past;
    size_t height;
    uint32_t minimum_block_version;
    uint32_t work_required;
};

bool operator==(const context& left, const context& right) NOEXCEPT;
bool operator!=(const context& left, const context& right) NOEXCEPT;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
