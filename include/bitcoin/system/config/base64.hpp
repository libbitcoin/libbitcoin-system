/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BASE64_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BASE64_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for base64 encoded data.
class BC_API base64 final
{
public:
    base64() NOEXCEPT;
    base64(data_chunk&& value) NOEXCEPT;
    base64(const data_chunk& value) NOEXCEPT;
    base64(const std::string& base64) THROWS;

    operator const data_chunk&() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        base64& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const base64& argument) NOEXCEPT;

private:
    data_chunk value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
