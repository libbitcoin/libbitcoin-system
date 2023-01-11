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
#ifndef LIBBITCOIN_SYSTEM_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_TRANSACTION_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for chain::script.
class BC_API transaction final
  : public chain::transaction
{
public:
    transaction() NOEXCEPT;
    transaction(chain::transaction&& value) NOEXCEPT;
    transaction(const chain::transaction& value) NOEXCEPT;
    transaction(const std::string& base16) THROWS;

    ////std::string to_string() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        transaction& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const transaction& argument) NOEXCEPT;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
