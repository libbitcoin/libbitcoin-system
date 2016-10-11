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
#include <bitcoin/bitcoin/utility/log.hpp>

namespace libbitcoin {
namespace log {

static std::map<libbitcoin::log::severity, std::string> severity_mapping = {
    { libbitcoin::log::severity::debug,   "DEBUG"   },
    { libbitcoin::log::severity::info,    "INFO"    },
    { libbitcoin::log::severity::warning, "WARNING" },
    { libbitcoin::log::severity::error,   "ERROR"   },
    { libbitcoin::log::severity::fatal,   "FATAL"   }
};

boost::log::formatting_ostream::ostream_type& operator<<(
    boost::log::formatting_ostream::ostream_type& stream,
    const severity level)
{
    stream << severity_mapping[level];
    return stream;
}

} // namespace log
} // namespace libbitcoin
