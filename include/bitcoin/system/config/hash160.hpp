/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_HASH160_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_HASH160_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a bitcoin 160 bit hash.
class BC_API hash160 final
{
public:
    hash160() NOEXCEPT;
    hash160(short_hash&& value) NOEXCEPT;
    hash160(const short_hash& value) NOEXCEPT;
    hash160(const std::string& base16) THROWS;

    operator const short_hash&() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        hash160& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const hash160& argument) NOEXCEPT;

private:
    short_hash value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
