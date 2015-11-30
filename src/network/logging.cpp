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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/network/logging.hpp>

#include <functional>
#include <iostream>
#include <mutex>
#include <utility>
#include <sstream>
#include <string>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>

namespace libbitcoin {
namespace network {

// Guard against concurrent stream writes.
static std::mutex console_mutex;

// Guard against concurrent file writes.
static std::mutex file_mutex;

static std::string make_log_string(log::level level, const std::string& domain,
    const std::string& body)
{
    if (body.empty())
        return "";

    static const auto form = "%1% %2% [%3%] %4%\n";
    const auto message = boost::format(form) %
        boost::posix_time::microsec_clock::local_time().time_of_day() %
        log::to_text(level) %
        domain %
        body;

    return message.str();
}

static void log_to_file(std::ofstream& file, log::level level,
    const std::string& domain, const std::string& body)
{
    std::string message;
    message = make_log_string(level, domain, body);

    if (!message.empty())
    {
        // This is overkill as we may locking across different files, but
        // since fatal/error/warning logging is very infrequent this is ok.
        
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock_file(file_mutex);

        file << message;
        file.flush();
        ///////////////////////////////////////////////////////////////////////
    }
}

static void log_to_both(std::ostream& device, std::ofstream& file,
    log::level level, const std::string& domain, const std::string& body)
{
    std::string message;
    message = make_log_string(level, domain, body);

    if (!message.empty())
    {
        // This is overkill as we may locking across different devices, but
        // since fatal/error/warning logging is very infrequent this is ok.
        // Also cout and cerr devices are typically writing to the same
        // display. Locking across both devices prevents presentation mixing.

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock_console(console_mutex);

        device << message;
        device.flush();
        ///////////////////////////////////////////////////////////////////////
    }

    if (!message.empty())
    {
        // This is overkill as we may locking across different files, but
        // since fatal/error/warning logging is very infrequent this is ok.

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock_file(file_mutex);

        file << message;
        file.flush();
        ///////////////////////////////////////////////////////////////////////
    }
}

static void output_file(std::ofstream& file, log::level level,
    const std::string& domain, const std::string& body)
{
    log_to_file(file, level, domain, body);
}

static void output_both(std::ofstream& file, std::ostream& output,
    log::level level, const std::string& domain, const std::string& body)
{
    log_to_both(output, file, level, domain, body);
}

static void error_file(std::ofstream& file, log::level level,
    const std::string& domain, const std::string& body)
{
    log_to_file(file, level, domain, body);
}

static void error_both(std::ofstream& file, std::ostream& error,
    log::level level, const std::string& domain, const std::string& body)
{
    log_to_both(error, file, level, domain, body);
}

void initialize_logging(std::ofstream& debug, std::ofstream& error,
    std::ostream& output_stream, std::ostream& error_stream)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    // debug|info => debug_log
    log::debug("").set_output_function(std::bind(output_file,
        std::ref(debug), _1, _2, _3));
    log::info("").set_output_function(std::bind(output_both,
        std::ref(debug), std::ref(output_stream), _1, _2, _3));

    // warning|error|fatal => error_log + console
    log::warning("").set_output_function(std::bind(error_file,
        std::ref(error), _1, _2, _3));
    log::error("").set_output_function(std::bind(error_both,
        std::ref(error), std::ref(error_stream), _1, _2, _3));
    log::fatal("").set_output_function(std::bind(error_both,
        std::ref(error), std::ref(error_stream), _1, _2, _3));
}

} // namespace network
} // namespace libbitcoin
