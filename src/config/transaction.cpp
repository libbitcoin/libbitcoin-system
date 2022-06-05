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
#include <bitcoin/system/config/transaction.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/config/base16.hpp>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

transaction::transaction() noexcept
  : value_()
{
}

transaction::transaction(chain::transaction&& value) noexcept
  : value_(std::move(value))
{
}

transaction::transaction(const chain::transaction& value) noexcept
  : value_(value)
{
}

transaction::transaction(const std::string& base16) noexcept(false)
{
    std::istringstream(base16) >> *this;
}

transaction::operator const chain::transaction&() const noexcept
{
    return value_;
}

std::istream& operator>>(std::istream& stream,
    transaction& argument) noexcept(false)
{
    std::string base16;
    stream >> base16;

    data_chunk bytes;
    if (!decode_base16(bytes, base16))
        throw istream_exception(base16);

    argument.value_ = chain::transaction{ bytes, true };

    if (!argument.value_.is_valid())
        throw istream_exception(base16);

    return stream;
}

std::ostream& operator<<(std::ostream& stream,
    const transaction& argument) noexcept
{
    stream << encode_base16(argument.value_.to_data(true));
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
