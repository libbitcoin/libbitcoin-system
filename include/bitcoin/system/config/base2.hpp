/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BASE2_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BASE2_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for base2 encoded data.
class BC_API base2
{
public:
    /// Defaults.
    base2(base2&&) = default;
    base2(const base2&) = default;
    base2& operator=(base2&&) = default;
    base2& operator=(const base2&) = default;
    ~base2() = default;

    /// Constructors.
    base2() NOEXCEPT;
    base2(binary&& value) NOEXCEPT;
    base2(const binary& value) NOEXCEPT;
    base2(const std::string& binary) THROWS;

    /// Operators.

    operator const binary&() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        base2& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const base2& argument) NOEXCEPT;

private:
    binary value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
