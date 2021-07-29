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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_CONFIG_COMPACT_FILTER_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_COMPACT_FILTER_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/message/message.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a compact filter.
class BC_API compact_filter
{
public:
    compact_filter();
    compact_filter(const compact_filter& other);
    compact_filter(const std::string& hexcode);
    compact_filter(const message::compact_filter& value);

    compact_filter& operator=(const compact_filter& other);
    compact_filter& operator=(message::compact_filter&& other);

    bool operator==(const compact_filter& other) const;

    operator const message::compact_filter&() const;

    std::string to_string() const;

    friend std::istream& operator>>(std::istream& input,
        compact_filter& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const compact_filter& argument);

private:
    message::compact_filter value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
