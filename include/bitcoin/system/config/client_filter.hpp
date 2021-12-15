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

#ifdef MOVE_TO_BX

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_CONFIG_CLIENT_FILTER_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_CLIENT_FILTER_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/messages.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a compact filter.
class BC_API client_filter
{
public:
    client_filter();
    client_filter(const client_filter& other);
    client_filter(const std::string& hexcode);
    client_filter(const messages::client_filter& value);

    client_filter& operator=(const client_filter& other);
    client_filter& operator=(messages::client_filter&& other);

    bool operator==(const client_filter& other) const;

    operator const messages::client_filter&() const;

    std::string to_string() const;

    friend std::istream& operator>>(std::istream& input,
        client_filter& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const client_filter& argument);

private:
    messages::client_filter value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif

#endif //MOVE_TO_BX
