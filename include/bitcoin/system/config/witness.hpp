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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_WITNESS_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_WITNESS_HPP

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for chain::witness.
class BC_API witness final
  : public chain::witness
{
public:
    witness() NOEXCEPT;
    witness(chain::witness&& value) NOEXCEPT;
    witness(const chain::witness& value) NOEXCEPT;
    witness(const std::string& mnemonic) THROWS;

    friend BC_API std::istream& operator>>(std::istream& stream,
        witness& argument) THROWS;
    friend BC_API std::ostream& operator<<(std::ostream& stream,
        const witness& argument) NOEXCEPT;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
