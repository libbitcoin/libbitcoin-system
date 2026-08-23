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
#include <bitcoin/system/wallet/psbt/entry.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace psbt {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

bool entry::from_data(reader& source) NOEXCEPT
{
    // A zero key length is the map terminator.
    const auto size = source.read_size();
    if (is_zero(size) || !source)
        return false;

    key = source.read_bytes(size);
    value = source.read_bytes(source.read_size());
    return !!source;
}

void entry::to_data(writer& sink) const NOEXCEPT
{
    sink.write_variable(key.size());
    sink.write_bytes(key);
    sink.write_variable(value.size());
    sink.write_bytes(value);
}

uint64_t entry::type() const NOEXCEPT
{
    stream::in::fast stream(key);
    read::bytes::fast source(stream);
    return source.read_variable();
}

data_chunk entry::keydata() const NOEXCEPT
{
    stream::in::fast stream(key);
    read::bytes::fast source(stream);
    source.read_variable();
    return source.read_bytes();
}

bool key_origin::from_value(const data_chunk& value) NOEXCEPT
{
    constexpr auto integer = sizeof(uint32_t);
    if (is_zero(value.size()) || !is_zero(value.size() % integer))
        return false;

    stream::in::fast stream(value);
    read::bytes::fast source(stream);
    fingerprint = source.read_4_bytes_little_endian();

    path.clear();
    for (auto count = sub1(value.size() / integer); !is_zero(count); --count)
        path.push_back(source.read_4_bytes_little_endian());

    return !!source;
}

data_chunk key_origin::to_value() const NOEXCEPT
{
    data_chunk value{};
    stream::out::data stream(value);
    write::bytes::ostream sink(stream);
    sink.write_4_bytes_little_endian(fingerprint);
    for (const auto& index: path)
        sink.write_4_bytes_little_endian(index);

    sink.flush();
    return value;
}

BC_POP_WARNING()

} // namespace psbt
} // namespace wallet
} // namespace system
} // namespace libbitcoin
