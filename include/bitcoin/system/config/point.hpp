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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_POINT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_POINT_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper to convert between text and an point.
class BC_API point
{
public:
    static const std::string delimiter;

    point() noexcept;
    point(const point& other) noexcept;
    point(const chain::point& value) noexcept;
    point(const std::string& tuple);

    operator const chain::point&() const noexcept;

    friend std::istream& operator>>(std::istream& input, point& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const point& argument) noexcept;

private:
    chain::point value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
