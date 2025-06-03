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
#include <bitcoin/system/chain/context.hpp>

#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/policy.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

bool operator==(const context& left, const context& right) NOEXCEPT
{
    return left.flags == right.flags
        && left.timestamp == right.timestamp
        && left.median_time_past == right.median_time_past
        && left.height == right.height
        && left.minimum_block_version == right.minimum_block_version
        && left.work_required == right.work_required;
}

bool operator!=(const context& left, const context& right) NOEXCEPT
{
    return !(left == right);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
