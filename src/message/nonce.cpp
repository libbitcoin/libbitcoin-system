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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

void nonce_base::reset()
{
    nonce = 0;
}

bool nonce_base::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool nonce_base::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    nonce = read_8_bytes(stream);
    result = !stream.fail();

    if (!result)
        reset();

    return result;
}

data_chunk nonce_base::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_8_bytes(nonce);
    return result;
}

uint64_t nonce_base::satoshi_size() const
{
    return nonce_base::satoshi_fixed_size();
}

uint64_t nonce_base::satoshi_fixed_size()
{
    return 8;
}

uint64_t ping::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

uint64_t pong::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
