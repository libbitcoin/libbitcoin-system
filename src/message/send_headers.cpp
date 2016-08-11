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
#include <bitcoin/bitcoin/message/send_headers.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string send_headers::command = "sendheaders";
const uint32_t send_headers::version_minimum = version::level::bip130;
const uint32_t send_headers::version_maximum = version::level::maximum;

send_headers send_headers::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    send_headers instance;
    instance.from_data(version, data);
    return instance;
}

send_headers send_headers::factory_from_data(uint32_t version,
    std::istream& stream)
{
    send_headers instance;
    instance.from_data(version, stream);
    return instance;
}

send_headers send_headers::factory_from_data(uint32_t version,
    reader& source)
{
    send_headers instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t send_headers::satoshi_fixed_size(uint32_t version)
{
    return 0;
}

send_headers::send_headers()
{
    reset();
}

bool send_headers::is_valid() const
{
    return !version_unsupported_;
}

void send_headers::reset()
{
    version_unsupported_ = false;
}

bool send_headers::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool send_headers::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool send_headers::from_data(uint32_t version, reader& source)
{
    reset();

    if (version < send_headers::version_minimum)
        version_unsupported_ = true;

    return !version_unsupported_;
}

data_chunk send_headers::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void send_headers::to_data(uint32_t version, std::ostream& stream) const
{
}

uint64_t send_headers::serialized_size(uint32_t version) const
{
    return send_headers::satoshi_fixed_size(version);
}

} // namspace message
} // namspace libbitcoin
