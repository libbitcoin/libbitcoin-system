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
#include <bitcoin/bitcoin/utility/log.hpp>

#include <iostream>
#include <utility>
#include <sstream>
#include <string>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>

namespace libbitcoin {

log::log(level value, const std::string& domain)
  : level_(value), domain_(domain)
{
}

// g++ bug in initializer list.
// It should be: stream_(std::move(other.stream_))
// gcc.gnu.org/bugzilla/show_bug.cgi?id=54316
log::log(log&& other)
  : level_(other.level_),
    domain_(std::move(other.domain_)),
    stream_(other.stream_.str())
{
}

log::~log()
{
    if (destinations_.count(level_) != 0)
        destinations_[level_](level_, domain_, stream_.str());
}

void log::set_output_function(functor value)
{
    destinations_[level_] = value;
}

void log::clear()
{
    destinations_.clear();
}

log log::debug(const std::string& domain)
{
    return log(level::debug, domain);
}

log log::info(const std::string& domain)
{
    return log(level::info, domain);
}

log log::warning(const std::string& domain)
{
    return log(level::warning, domain);
}

log log::error(const std::string& domain)
{
    return log(level::error, domain);
}

log log::fatal(const std::string& domain)
{
    return log(level::fatal, domain);
}

std::string log::to_text(level value)
{
    switch (value)
    {
        case level::debug:
            return "DEBUG";
        case level::info:
            return "INFO";
        case level::warning:
            return "WARNING";
        case level::error:
            return "ERROR";
        case level::fatal:
            return "FATAL";
        case level::null:
            return "NULL";
        default:
            return "";
    }
}

void log::to_stream(std::ostream& out, level value, const std::string& domain,
    const std::string& body)
{
    std::ostringstream buffer;
    buffer << to_text(value);
    if (!domain.empty())
        buffer << " [" << domain << "]";

    buffer << ": " << body << std::endl;

    // Buffering prevents line interleaving across threads.
    out << buffer.str();
    out.flush();
}

void log::output_ignore(level, const std::string&, const std::string&)
{
}

void log::output_cout(level value, const std::string& domain,
    const std::string& body)
{
    to_stream(bc::cout, value, domain, body);
}

void log::output_cerr(level value, const std::string& domain,
    const std::string& body)
{
    to_stream(bc::cerr, value, domain, body);
}

log::destinations log::destinations_
{
#ifdef NDEBUG
    std::make_pair(level::debug, output_ignore),
#else
    std::make_pair(level::debug, output_cout),
#endif
    std::make_pair(level::info, output_cout),
    std::make_pair(level::warning, output_cerr),
    std::make_pair(level::error, output_cerr),
    std::make_pair(level::fatal, output_cerr)
};

} // namespace libbitcoin
