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
#include <map>
#include <sstream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

class BC_API log
{
public:
    enum class level
    {
        debug,
        info,
        warning,
        error,
        fatal,
        null
    };

    typedef std::function<void(level, const std::string&, const std::string&)>
        functor;

    log(level value, const std::string& domain);
    log(log&& other);
    ~log();

    /// Clear all log configuration.
    static void clear();

    /// Convert the log level value to English text.
    static std::string to_text(level value);

    // Stream to these functions.
    static log debug(const std::string& domain);
    static log info(const std::string& domain);
    static log warning(const std::string& domain);
    static log error(const std::string& domain);
    static log fatal(const std::string& domain);

    template <typename Type>
    log& operator<<(Type const& value)
    {
        stream_ << value;
        return *this;
    }

    /// Set the output functor for this log instance.
    void set_output_function(functor value);

private:
    typedef std::map<level, functor> destinations;

    static void output_ignore(level value, const std::string& domain,
        const std::string& body);
    static void output_cout(level value, const std::string& domain,
        const std::string& body);
    static void output_cerr(level value, const std::string& domain,
        const std::string& body);

    static void to_stream(std::ostream& out, level value,
        const std::string& domain, const std::string& body);

    static destinations destinations_;

    level level_;
    std::string domain_;
    std::ostringstream stream_;
};

} // namespace libbitcoin

#endif
