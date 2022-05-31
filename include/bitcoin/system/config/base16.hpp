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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BASE16_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BASE16_HPP

#include <array>
#include <iostream>
#include <string>
#include <cstdint>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for base16 encoded data.
class BC_API base16
{
public:
    /// Defaults.
    base16(base16&&) = default;
    base16(const base16&) = default;
    base16& operator=(base16&&) = default;
    base16& operator=(const base16&) = default;
    ~base16() = default;

    /// Constructors.

    base16() noexcept;
    base16(data_chunk&& value) noexcept;
    base16(const data_chunk& value) noexcept;
    base16(const std::string& base16) noexcept(false);

    template<size_t Size>
    base16(const data_array<Size>& value) noexcept
      : value_(value.begin(), value.end())
    {
    }

    // Operators.
    
    // unsafe
    ////operator data_slice() const noexcept;
    operator const data_chunk&() const noexcept;

    friend std::istream& operator>>(std::istream& stream,
        base16& argument) noexcept(false);
    friend std::ostream& operator<<(std::ostream& stream,
        const base16& argument) noexcept;

private:
    data_chunk value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
