/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/send_compact_blocks.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string send_compact_blocks::command = "sendcmpct";
const uint32_t send_compact_blocks::version_minimum = version::level::bip152;
const uint32_t send_compact_blocks::version_maximum = version::level::bip152;

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, const data_chunk& data)
{
    send_compact_blocks instance;
    instance.from_data(version, data);
    return instance;
}

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, std::istream& stream)
{
    send_compact_blocks instance;
    instance.from_data(version, stream);
    return instance;
}

send_compact_blocks send_compact_blocks::factory_from_data(
    const uint32_t version, reader& source)
{
    send_compact_blocks instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t send_compact_blocks::satoshi_fixed_size(uint32_t version)
{
    return 9;
}

send_compact_blocks::send_compact_blocks()
  : high_bandwidth_mode_(false), version_(0)
{
}

send_compact_blocks::send_compact_blocks(bool high_bandwidth_mode,
    uint64_t version)
  : high_bandwidth_mode_(high_bandwidth_mode),
    version_(version)
{
}

send_compact_blocks::send_compact_blocks(const send_compact_blocks& other)
  : send_compact_blocks(other.high_bandwidth_mode_, other.version_)
{
}

send_compact_blocks::send_compact_blocks(send_compact_blocks&& other)
  : send_compact_blocks(other.high_bandwidth_mode_, other.version_)
{
}

bool send_compact_blocks::is_valid() const
{
    return (version_ != 0);
}

void send_compact_blocks::reset()
{
    high_bandwidth_mode_ = false;
    version_ = 0;
}

bool send_compact_blocks::from_data(uint32_t version,
    const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool send_compact_blocks::from_data(uint32_t version,
    std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool send_compact_blocks::from_data(uint32_t version,
    reader& source)
{
    reset();

    const auto mode = source.read_byte();

    if (mode > 1)
        source.invalidate();

    high_bandwidth_mode_ = (mode == 1);
    this->version_ = source.read_8_bytes_little_endian();

    if (version < send_compact_blocks::version_minimum)
        source.invalidate();

    if (!source)
        reset();

    return source;
}

data_chunk send_compact_blocks::to_data(uint32_t version) const
{
    data_chunk data;
    data.reserve(serialized_size(version));
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void send_compact_blocks::to_data(uint32_t version,
    std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void send_compact_blocks::to_data(uint32_t version,
    writer& sink) const
{
    sink.write_byte(static_cast<uint8_t>(high_bandwidth_mode_));
    sink.write_8_bytes_little_endian(this->version_);
}

uint64_t send_compact_blocks::serialized_size(uint32_t version) const
{
    return send_compact_blocks::satoshi_fixed_size(version);
}

bool send_compact_blocks::high_bandwidth_mode() const
{
    return high_bandwidth_mode_;
}

void send_compact_blocks::set_high_bandwidth_mode(bool mode)
{
    high_bandwidth_mode_ = mode;
}

uint64_t send_compact_blocks::version() const
{
    return version_;
}

void send_compact_blocks::set_version(uint64_t version)
{
    version_ = version;
}

send_compact_blocks& send_compact_blocks::operator=(send_compact_blocks&& other)
{
    high_bandwidth_mode_ = other.high_bandwidth_mode_;
    version_ = other.version_;
    return *this;
}

bool send_compact_blocks::operator==(const send_compact_blocks& other) const
{
    return (high_bandwidth_mode_ == other.high_bandwidth_mode_) &&
        (version_ == other.version_);
}

bool send_compact_blocks::operator!=(const send_compact_blocks& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace libbitcoin
