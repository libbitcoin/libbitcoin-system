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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_HASH256_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_HASH256_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a bitcoin 256 bit hash.
class BC_API hash256 final
{
public:
    typedef std::vector<hash256> list;

    hash256() NOEXCEPT;
    hash256(hash_digest&& value) NOEXCEPT;
    hash256(const hash_digest& value) NOEXCEPT;
    hash256(const std::string& base16) THROWS;

    std::string to_string() const NOEXCEPT;

    operator const hash_digest&() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        hash256& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const hash256& argument) NOEXCEPT;

private:
    hash_digest value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
