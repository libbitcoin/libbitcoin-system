/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_LOGGER_HPP
#define LIBBITCOIN_LOGGER_HPP

#include <functional>
#include <map>
#include <sstream>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

enum class log_level
{
    null,
    debug,
    info,
    warning,
    error,
    fatal
};

BC_API std::string level_repr(log_level level);

class logger_wrapper
{
public:
    typedef std::function<void (
        log_level, const std::string&, const std::string&)> logger_output_func;

    logger_wrapper(log_level level, const std::string& token);
    logger_wrapper(logger_wrapper&& other);
    BC_API ~logger_wrapper();

    template <typename T>
    logger_wrapper& operator<<(T const& value) 
    {
        stream_ << value;
        return *this;
    }

    BC_API void set_output_function(logger_output_func func);

private:
    typedef std::map<log_level, logger_output_func> destination_map;

    static destination_map dests_;

    log_level level_;
    std::string domain_;
    std::ostringstream stream_;
};

BC_API logger_wrapper log_debug(const std::string& domain="");
BC_API logger_wrapper log_info(const std::string& domain="");
BC_API logger_wrapper log_warning(const std::string& domain="");
BC_API logger_wrapper log_error(const std::string& domain="");
BC_API logger_wrapper log_fatal(const std::string& domain="");

} // namespace libbitcoin

#endif

