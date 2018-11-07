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
#include <bitcoin/bitcoin/log/sink.hpp>

#include <map>
#include <string>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/log/attributes.hpp>
#include <bitcoin/bitcoin/log/file_collector_repository.hpp>
#include <bitcoin/bitcoin/log/severity.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

namespace libbitcoin {
namespace log {

using namespace boost::log;
using namespace boost::log::expressions;
using namespace boost::log::keywords;
using namespace boost::log::sinks;
using namespace boost::log::sinks::file;
using namespace boost::posix_time;

#define TIME_FORMAT "%H:%M:%S.%f"
#define TIME_STAMP attributes::timestamp.get_name()
#define TIME_FORMATTER format_date_time<ptime, char>(TIME_STAMP, TIME_FORMAT)
#define SEVERITY_FORMATTER attributes::severity
#define CHANNEL_FORMATTER "[" << attributes::channel << "]"
#define MESSAGE_FORMATTER smessage

#define LINE_FORMATTER boost::log::expressions::stream \
    << TIME_FORMATTER << " " \
    << SEVERITY_FORMATTER << " " \
    << CHANNEL_FORMATTER << " " \
    << MESSAGE_FORMATTER

typedef synchronous_sink<text_file_backend> text_file_sink;
typedef synchronous_sink<text_ostream_backend> text_stream_sink;

static const auto base_filter =
    has_attr(attributes::channel) &&
    has_attr(attributes::severity) &&
    has_attr(attributes::timestamp);

static const auto error_filter = base_filter && (
    (attributes::severity == severity::warning) ||
    (attributes::severity == severity::error) ||
    (attributes::severity == severity::fatal));

static const auto info_filter = base_filter &&
    (attributes::severity == severity::info);

static const auto lean_filter = base_filter &&
    (attributes::severity != severity::verbose);

static std::map<severity, std::string> severity_mapping
{
    { severity::verbose, "VERBOSE" },
    { severity::debug, "DEBUG" },
    { severity::info, "INFO" },
    { severity::warning, "WARNING" },
    { severity::error, "ERROR" },
    { severity::fatal, "FATAL" }
};

formatter& operator<<(formatter& stream, severity value)
{
    stream << severity_mapping[value];
    return stream;
}

static boost::shared_ptr<collector> file_collector(
    const rotable_file& rotation)
{
    // rotation_size controls enable/disable so use zero as max sentinel.
    return bc::log::make_collector(
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

        // Upon restart, scan the directory for files matching the pattern.
        backend->scan_for_files();
    }

    // Flush the sink after each logical line.
    backend->auto_flush(true);

    // Add the formatter to the sink.
    sink->set_formatter(LINE_FORMATTER);

    // Register the sink with the logging core.
    core::get()->add_sink(sink);
    return sink;
}

template<typename Stream>
static boost::shared_ptr<text_stream_sink> add_text_stream_sink(
    boost::shared_ptr<Stream>& stream)
{
    // Construct a log sink.
    const auto sink = boost::make_shared<text_stream_sink>();
    const auto backend = sink->locked_backend();

    // Add a stream for the sink to write to.
    backend->add_stream(stream);

    // Flush the sink after each logical line.
    backend->auto_flush(true);

    // Add the formatter to the sink.
    sink->set_formatter(LINE_FORMATTER);

    // Register the sink with the logging core.
    core::get()->add_sink(sink);
    return sink;
}

void initialize()
{
    struct null_stream : public std::ostream
    {
        struct null_buffer : public std::streambuf
        {
            int overflow(int value) { return value; }
        };

        null_buffer buffer_;
        null_stream() : std::ostream(&buffer_) {}
    };

    // Null stream instances used to disable log output.
    static auto debug_file = boost::make_shared<bc::ofstream>("/dev/null");
    static auto error_file = boost::make_shared<bc::ofstream>("/dev/null");
    static log::stream output_stream = boost::make_shared<null_stream>();
    static log::stream error_stream = boost::make_shared<null_stream>();
    initialize(debug_file, error_file, output_stream, error_stream, false);
}

void initialize(log::file& debug_file, log::file& error_file,
    log::stream& output_stream, log::stream& error_stream, bool verbose)
{
    if (verbose)
        add_text_stream_sink(debug_file)->set_filter(base_filter);
    else
        add_text_stream_sink(debug_file)->set_filter(lean_filter);

    add_text_stream_sink(error_file)->set_filter(error_filter);
    add_text_stream_sink(output_stream)->set_filter(info_filter);
    add_text_stream_sink(error_stream)->set_filter(error_filter);
}

void initialize(const rotable_file& debug_file, const rotable_file& error_file,
    log::stream& output_stream, log::stream& error_stream, bool verbose)
{
    if (verbose)
        add_text_file_sink(debug_file)->set_filter(base_filter);
    else
        add_text_file_sink(debug_file)->set_filter(lean_filter);

    add_text_file_sink(error_file)->set_filter(error_filter);
    add_text_stream_sink(output_stream)->set_filter(info_filter);
    add_text_stream_sink(error_stream)->set_filter(error_filter);
}

} // namespace log
} // namespace libbitcoin
