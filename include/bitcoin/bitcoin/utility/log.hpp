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
#ifndef LIBBITCOIN_LOG_HPP
#define LIBBITCOIN_LOG_HPP

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>

// libbitcoin defines the log and tracking but does not use them.
// These are defined in bc so that they can be used in network and blockchain.

namespace libbitcoin {
namespace log {

enum class severity
{
    debug,
    info,
    warning,
    error,
    fatal
};

boost::log::formatting_ostream::ostream_type& operator<<(
    boost::log::formatting_ostream::ostream_type& stream,
    const severity level);

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", libbitcoin::log::severity)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

}

typedef boost::log::sources::severity_channel_logger_mt<
    severity, std::string> severity_channel_source_mt;

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(source, severity_channel_source_mt)
{
    severity_channel_source_mt logger;
    logger.add_attribute(libbitcoin::log::attributes::timestamp.get_name(),
        boost::log::attributes::utc_clock());
    return logger;
}

#define LOG_DEBUG(module) \
    BOOST_LOG_CHANNEL_SEV(libbitcoin::log::source::get(), module, libbitcoin::log::severity::debug)

#define LOG_INFO(module) \
    BOOST_LOG_CHANNEL_SEV(libbitcoin::log::source::get(), module, libbitcoin::log::severity::info)

#define LOG_WARNING(module) \
    BOOST_LOG_CHANNEL_SEV(libbitcoin::log::source::get(), module, libbitcoin::log::severity::warning)

#define LOG_ERROR(module) \
    BOOST_LOG_CHANNEL_SEV(libbitcoin::log::source::get(), module, libbitcoin::log::severity::error)

#define LOG_FATAL(module) \
    BOOST_LOG_CHANNEL_SEV(libbitcoin::log::source::get(), module, libbitcoin::log::severity::fatal)

} // namespace log
} // namespace libbitcoin

#endif
