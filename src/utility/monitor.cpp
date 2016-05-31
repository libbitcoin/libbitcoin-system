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
#include <bitcoin/bitcoin/utility/monitor.hpp>

#include <cstddef>
#include <string>
#include <bitcoin/bitcoin/utility/log.hpp>

namespace libbitcoin {

monitor::monitor(count_ptr counter, const std::string& name)
  : counter_(counter), name_(name)
{
    trace(++(*counter_), "+");
}

monitor::~monitor()
{
    trace(--(*counter_), "-");
}

void monitor::trace(size_t count, const std::string& action) const
{
#ifndef NDEBUG
    ////log::debug(LOG_SYSTEM) << action << " " << name_ << " {" << count << "}";
#endif
}

} // namespace libbitcoin
