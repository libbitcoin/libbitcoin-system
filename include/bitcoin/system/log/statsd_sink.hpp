/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_LOG_STATSD_SINK_HPP
#define LIBBITCOIN_SYSTEM_LOG_STATSD_SINK_HPP

#include <boost/asio.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/log/rotable_file.hpp>
#include <bitcoin/system/utility/threadpool.hpp>

namespace libbitcoin {
namespace system {
namespace log {

void initialize_statsd(const rotable_file& file);

void initialize_statsd(threadpool& pool, const config::authority& server);

} // namespace log
} // namespace system
} // namespace libbitcoin

#endif
