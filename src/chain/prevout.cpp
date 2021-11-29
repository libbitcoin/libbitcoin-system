/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/prevout.hpp>

#include <utility>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/position.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Default prevout ensures consensus invalidity as output is invalid, value
// exceeds maximum money, and position is genesis height and maximum time.
prevout::prevout()
  : output{ max_uint64, {}, false }, position{ zero, max_uint32 }
{
}

prevout::prevout(output&& output, position&& position)
  : output(std::move(output)), position(std::move(position))
{
}

prevout::prevout(const output& output, const position& position)
  : output(output), position(position)
{
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
