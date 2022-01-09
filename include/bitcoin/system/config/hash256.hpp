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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_HASH256_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_HASH256_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a bitcoin 256 bit hash.
class BC_API hash256
{
public:
    typedef std::vector<hash256> list;

    hash256() noexcept;
    hash256(const hash256& other) noexcept;
    hash256(const hash_digest& value) noexcept;
    hash256(const std::string& hexcode);

    std::string to_string() const noexcept;

    operator const hash_digest&() const noexcept;

    friend std::istream& operator>>(std::istream& input, hash256& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const hash256& argument) noexcept;

private:
    hash_digest value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
