/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BYTE_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BYTE_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Container for a byte, as the stream reads uint8_t as a character.
class BC_API byte
{
public:
    typedef uint8_t type;

    DEFAULT_COPY_MOVE_DESTRUCT(byte);

    byte() NOEXCEPT;
    byte(type value) NOEXCEPT;

    /// Deserialize from decimal string (throws on invalid format).
    byte(const std::string& value) THROWS;

    operator type() const NOEXCEPT;

    /// Deserialize from input stream (throws on invalid format).
    friend std::istream& operator>>(std::istream& input, byte& argument) THROWS;

    /// Serialize to output stream.
    friend std::ostream& operator<<(std::ostream& output,
        const byte& argument) NOEXCEPT;

private:
    type value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
