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
#include <bitcoin/system/config/base58check.hpp>

#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base58check::base58check() NOEXCEPT
{
}

base58check::base58check(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base58check::base58check(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base58check::base58check(const std::string& base58check) THROWS
{
    std::istringstream(base58check) >> *this;
}

base58check::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base58check& argument) THROWS
{
    std::string base58check;
    stream >> base58check;

    data_chunk checked{};
    if (!decode_base58(checked, base58check) || !verify_checksum(checked))
        throw istream_exception(base58check);

    checked.resize(checked.size() - checksum_default_size);
    argument.value_ = std::move(checked);
    return stream;
}

std::ostream& operator<<(std::ostream& stream,
    const base58check& argument) NOEXCEPT
{
    stream << encode_base58(append_checksum({ argument.value_ }));
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
