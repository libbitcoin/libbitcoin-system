/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/log/statsd_sink.hpp>

#include <map>
#include <string>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/log/features/counter.hpp>
#include <bitcoin/system/log/features/gauge.hpp>
#include <bitcoin/system/log/features/metric.hpp>
#include <bitcoin/system/log/features/rate.hpp>
#include <bitcoin/system/log/features/timer.hpp>
#include <bitcoin/system/log/file_collector_repository.hpp>
#include <bitcoin/system/log/severity.hpp>
#include <bitcoin/system/log/udp_client_sink.hpp>
#include <bitcoin/system/unicode/ofstream.hpp>
#include <bitcoin/system/utility/asio.hpp>
#include <bitcoin/system/utility/threadpool.hpp>

namespace libbitcoin {
namespace system {
namespace log {

using namespace bc::system::config;
using namespace boost::asio::ip;
using namespace boost::log;
using namespace boost::log::expressions;
using namespace boost::log::sinks;
using namespace boost::log::sinks::file;

typedef synchronous_sink<text_file_backend> text_file_sink;
typedef synchronous_sink<udp_client_sink> text_udp_sink;

static const auto statsd_filter = has_attr(attributes::metric) &&
    (has_attr(attributes::counter) || has_attr(attributes::gauge) ||
        has_attr(attributes::timer));

void statsd_formatter(const record_view& record, formatting_ostream& stream)
{
    // Get the LineID attribute value and put it into the stream.
    stream << record[attributes::metric] << ":";

    if (has_attribute<int64_t>(attributes::counter.get_name())(record))
        stream << record[attributes::counter] << "|c";

    if (has_attribute<uint64_t>(attributes::gauge.get_name())(record))
        stream << record[attributes::gauge] << "|g";

    if (has_attribute<asio::milliseconds>(attributes::timer.get_name())(record))
        stream << record[attributes::timer].get().count() << "|ms";

    if (has_attribute<float>(attributes::rate.get_name())(record))
        stream << "|@" << record[attributes::rate];
}

static boost::shared_ptr<collector> file_collector(
    const rotable_file& rotation)
{
    // rotation_size controls enable/disable so use zero as max sentinel.
    return log::make_collector(
        rotation.archive_directory,
        rotation.maximum_archive_size == 0 ? max_size_t :
            rotation.maximum_archive_size,
        rotation.minimum_free_space,
        rotation.maximum_archive_files == 0 ? max_size_t :
            rotation.maximum_archive_files);
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
    core::get()->add_sink(sink);
    return sink;
}

void initialize_statsd(const rotable_file& file)
{
    add_text_file_sink(file)->set_filter(statsd_filter);
}

static boost::shared_ptr<text_udp_sink> add_udp_sink(threadpool& pool,
    const authority& server)
{
    auto socket = boost::make_shared<udp::socket>(pool.service());
    socket->open(udp::v6());

    auto endpoint = boost::make_shared<udp::endpoint>(server.asio_ip(),
        server.port());

    // Construct a log sink.
    const auto backend = boost::make_shared<udp_client_sink>(socket, endpoint);
    const auto sink = boost::make_shared<text_udp_sink>(backend);

    // Add the formatter to the sink.
    sink->set_formatter(&statsd_formatter);

    // Register the sink with the logging core.
    core::get()->add_sink(sink);
    return sink;
}

void initialize_statsd(threadpool& pool, const authority& server)
{
    if (server)
        add_udp_sink(pool, server)->set_filter(statsd_filter);
}

} // namespace log
} // namespace system
} // namespace libbitcoin
