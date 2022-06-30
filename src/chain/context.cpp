/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/policy.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

bool context::is_enabled(chain::forks fork) const NOEXCEPT
{
    return to_bool(fork & forks);
}

bool context::is_policy(chain::policy police) const NOEXCEPT
{
    return to_bool(police & policy);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
