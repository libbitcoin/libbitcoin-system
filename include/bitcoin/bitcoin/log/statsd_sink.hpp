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
#ifndef LIBBITCOIN_LOG_STATSD_SINK_HPP
#define LIBBITCOIN_LOG_STATSD_SINK_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/log/rotable_file.hpp>

namespace libbitcoin {
namespace log {

//void statsd_formatter(boost::log::record_view const& record,
//    boost::log::formatting_ostream& stream);

/// Initializes default rotable libbitcoin logging sinks and formats.
void initialize_statsd(const rotable_file& file);

} // namespace log
} // namespace libbitcoin

#endif
