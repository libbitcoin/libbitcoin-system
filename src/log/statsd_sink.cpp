/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/log/statsd_sink.hpp>

#include <map>
#include <string>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <bitcoin/bitcoin/log/features/counter.hpp>
#include <bitcoin/bitcoin/log/features/gauge.hpp>
#include <bitcoin/bitcoin/log/features/metric.hpp>
#include <bitcoin/bitcoin/log/features/rate.hpp>
#include <bitcoin/bitcoin/log/features/timer.hpp>
#include <bitcoin/bitcoin/log/file_collector_repository.hpp>
#include <bitcoin/bitcoin/log/severity.hpp>
#include <bitcoin/bitcoin/log/udp_client_sink.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

namespace libbitcoin {
namespace log {

namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;

typedef sinks::synchronous_sink<sinks::text_file_backend> text_file_sink;
typedef sinks::synchronous_sink<udp_client_sink> text_udp_sink;

const auto statsd_filter = expr::has_attr(attributes::metric) &&
    (expr::has_attr(attributes::counter) ||
        expr::has_attr(attributes::gauge) ||
        expr::has_attr(attributes::timer));

void statsd_formatter(boost::log::record_view const& record,
    boost::log::formatting_ostream& stream)
{
    // Get the LineID attribute value and put it into the stream
    stream << record[attributes::metric] << ":";

    if (expr::has_attribute<int64_t>(attributes::counter.get_name())(record))
        stream << record[attributes::counter] << "|c";

    if (expr::has_attribute<uint64_t>(attributes::gauge.get_name())(record))
        stream << record[attributes::gauge] << "|g";

    if (expr::has_attribute<std::chrono::milliseconds>(attributes::timer.get_name())(record))
    {
        auto value = record[attributes::timer].get().count();
        stream << value << "|ms";
    }

    if (expr::has_attribute<float>(attributes::rate.get_name())(record))
        stream << "|@" << record[attributes::rate];
}

static boost::shared_ptr<sinks::file::collector> file_collector(
    const rotable_file& rotation)
{
    return bc::log::make_collector(
        rotation.archive_directory,
        rotation.maximum_files_size,
        rotation.minimum_free_space,
        rotation.maximum_files);
}

static boost::shared_ptr<text_file_sink> add_text_file_sink(
    const rotable_file& rotation)
{
    // Construct a log sink.
    const auto sink = boost::make_shared<text_file_sink>();
    const auto backend = sink->locked_backend();

    // Add a file stream for the sink to write to.
    backend->set_file_name_pattern(rotation.original_log);

    // Set archival parameters.
    if (rotation.rotation_size != 0)
    {
        backend->set_rotation_size(rotation.rotation_size);
        backend->set_file_collector(file_collector(rotation));
    }

    // Flush the sink after each logical line.
    backend->auto_flush(true);

    // Add the formatter to the sink.
    sink->set_formatter(&statsd_formatter);

    // Register the sink with the logging core.
    boost::log::core::get()->add_sink(sink);
    return sink;
}

void initialize_statsd(const rotable_file& file)
{
    add_text_file_sink(file)->set_filter(statsd_filter);
}


static boost::shared_ptr<text_udp_sink> add_udp_sink(
    boost::asio::io_service& service, const bc::config::authority& server)
{
    auto socket = boost::make_shared<boost::asio::ip::udp::socket>(service);
    socket->open(boost::asio::ip::udp::v6());

    auto endpoint = boost::make_shared<boost::asio::ip::udp::endpoint>(
        server.asio_ip(), server.port());

    // Construct a log sink.
    const auto backend = boost::make_shared<udp_client_sink>(socket, endpoint);
    const auto sink = boost::make_shared<text_udp_sink>(backend);

    // Add the formatter to the sink.
    sink->set_formatter(&statsd_formatter);

    // Register the sink with the logging core.
    boost::log::core::get()->add_sink(sink);
    return sink;
}

void initialize_statsd(boost::asio::io_service& service,
    const bc::config::authority& server)
{
    add_udp_sink(service, server)->set_filter(statsd_filter);
}

} // namespace log
} // namespace libbitcoin
