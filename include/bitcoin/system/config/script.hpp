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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_SCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_SCRIPT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for chain::script.
class BC_API script final
  : public chain::script
{
public:
    script() NOEXCEPT;
    script(chain::script&& value) NOEXCEPT;
    script(const chain::script& value) NOEXCEPT;

    /// Split or unsplit tokens.
    script(const std::string& mnemonic) THROWS;
    script(const std_vector<std::string>& tokens) THROWS;

    /// Default text encoding is mnemonic, so provide data for base16.
    script(const data_chunk& value) NOEXCEPT;

    ////std::string to_string() const NOEXCEPT;

    friend std::istream& operator>>(std::istream& stream,
        script& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const script& argument) NOEXCEPT;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
