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
#include <bitcoin/system/config/witness.hpp>

#include <sstream>
#include <bitcoin/system/chain/chain.hpp>

namespace libbitcoin {
namespace system {
namespace config {

witness::witness() NOEXCEPT
  : chain::witness()
{
}

witness::witness(chain::witness&& value) NOEXCEPT
  : chain::witness(std::move(value))
{
}

witness::witness(const chain::witness& value) NOEXCEPT
  : chain::witness(value)
{
}

witness::witness(const std::string& mnemonic) THROWS
{
    std::istringstream(mnemonic) >> *this;
}

std::istream& operator>>(std::istream& stream, witness& argument) THROWS
{
    std::istreambuf_iterator<char> end;
    std::string mnemonic(std::istreambuf_iterator<char>(stream), end);

    argument = chain::witness{ mnemonic };
    if (!argument.is_valid())
        throw istream_exception(mnemonic);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const witness& argument) NOEXCEPT
{
    stream << argument.to_string();
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
