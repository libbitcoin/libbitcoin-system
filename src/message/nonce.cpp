/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/nonce.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {
namespace message {

bool nonce_base::is_valid() const
{
    return (nonce != 0);
}

void nonce_base::reset()
{
    nonce = 0;
}

bool nonce_base::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool nonce_base::from_data(std::istream& stream)
{
    reset();

    nonce = read_8_bytes(stream);

    if (!stream)
        reset();

    return stream;
}

data_chunk nonce_base::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void nonce_base::to_data(std::ostream& stream) const
{
    write_8_bytes(stream, nonce);
}

uint64_t nonce_base::satoshi_size() const
{
    return nonce_base::satoshi_fixed_size();
}

uint64_t nonce_base::satoshi_fixed_size()
{
    return 8;
}

ping ping::factory_from_data(const data_chunk& data)
{
    ping instance;
    instance.from_data(data);
    return instance;
}

ping ping::factory_from_data(std::istream& stream)
{
    ping instance;
    instance.from_data(stream);
    return instance;
}

uint64_t ping::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

pong pong::factory_from_data(const data_chunk& data)
{
    pong instance;
    instance.from_data(data);
    return instance;
}

pong pong::factory_from_data(std::istream& stream)
{
    pong instance;
    instance.from_data(stream);
    return instance;
}

uint64_t pong::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
