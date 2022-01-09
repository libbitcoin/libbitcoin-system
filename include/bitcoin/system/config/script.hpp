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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_SCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_SCRIPT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper to convert between base16/raw script and script_type.
class BC_API script
{
public:
    script() noexcept;
    script(const script& other) noexcept;
    script(const chain::script& value) noexcept;
    script(const data_chunk& value) noexcept;
    script(const std::vector<std::string>& tokens);
    script(const std::string& mnemonic);

    /// Serialize the script to bytes according to the p2p wire protocol.
    data_chunk to_data() const noexcept;

    /// Return a pretty-printed copy of the script.
    std::string to_string(
        uint32_t flags=chain::forks::all_rules) const noexcept;

    operator const chain::script&() const noexcept;

    friend std::istream& operator>>(std::istream& input, script& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const script& argument) noexcept;

private:
    chain::script value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
