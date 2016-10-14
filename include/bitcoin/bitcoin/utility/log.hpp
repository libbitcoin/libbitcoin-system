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

#include <iostream>
#include <string>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

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

typedef boost::shared_ptr<bc::ofstream> file;
typedef boost::shared_ptr<std::ostream> stream;
typedef boost::log::formatting_ostream::ostream_type formatter;
typedef boost::log::sources::severity_channel_logger_mt<severity, std::string>
    severity_source;

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", bc::log::severity)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)

}

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(source, severity_source)
{
    static const auto name = attributes::timestamp.get_name();
    severity_source logger;
    logger.add_attribute(name, boost::log::attributes::utc_clock());
    return logger;
}

#define BC_LOG_SEVERITY(id, level) \
    BOOST_LOG_CHANNEL_SEV(bc::log::source::get(), id, bc::log::severity::level)

#define LOG_DEBUG(module) BC_LOG_SEVERITY(module, debug)
#define LOG_INFO(module) BC_LOG_SEVERITY(module, info)
#define LOG_WARNING(module) BC_LOG_SEVERITY(module, warning)
#define LOG_ERROR(module) BC_LOG_SEVERITY(module, error)
#define LOG_FATAL(module) BC_LOG_SEVERITY(module, fatal)

/// Initializes default libbitcoin logging sinks and formats.
void initialize(log::file& debug_file, log::file& error_file,
    log::stream& output_stream, log::stream& error_stream);

/// Log stream operator.
formatter& operator<<(formatter& stream, severity level);

} // namespace log
} // namespace libbitcoin

#endif
