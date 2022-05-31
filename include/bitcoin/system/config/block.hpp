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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BLOCK_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BLOCK_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for chain::block.
class BC_API block
{
public:
    /// Defaults.
    block(block&&) = default;
    block(const block&) = default;
    block& operator=(block&&) = default;
    block& operator=(const block&) = default;
    ~block() = default;

    /// Constructors.
    block() noexcept;
    block(chain::block&& value) noexcept;
    block(const chain::block& value) noexcept;
    block(const std::string& base16) noexcept(false);

    std::string to_string() const noexcept;

    /// Operators.

    block& operator=(chain::block&& value) noexcept;
    block& operator=(const chain::block& value) noexcept;
    bool operator==(const block& other) const noexcept;

    operator const chain::block&() const noexcept;

    friend std::istream& operator>>(std::istream& stream,
        block& argument) noexcept(false);
    friend std::ostream& operator<<(std::ostream& stream,
        const block& argument) noexcept;

private:
    chain::block value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
