/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_PREVOUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_PREVOUT_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API prevout final
{
public:
    /// chain/node: prevout is of a coinbase tx.
    bool coinbase{ true };

    /// chain: confirmed prevout mtp or max_uint32.
    uint32_t median_time_past{ max_uint32 };

    /// chain: confirmed prevout height or max_uint32.
    size_t prevout_height{ max_uint32 };

    /// chain: confirmed spender height or max_uint32.
    uint32_t spender_height{ max_uint32 };

    /// node: database identifier for the parent tx.
    /// Link::terminal must align with link defaults of max_uint32.
    uint32_t parent_tx{ max_uint32 };

    /// node: database identifier for input/point.
    /// Link::terminal must align with link defaults of max_uint32.
    uint32_t point_link{ max_uint32 };
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
