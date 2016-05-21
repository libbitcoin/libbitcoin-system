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
#include <bitcoin/bitcoin/message/nonce.hpp>

#include <cstdint>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

nonce_::nonce_(const uint64_t nonce)
{
    this->nonce = nonce;
}

bool nonce_::is_valid() const
{
    return (nonce != 0);
}

void nonce_::reset()
{
    nonce = 0;
}

bool nonce_::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool nonce_::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool nonce_::from_data(reader& source)
{
    reset();

    nonce = source.read_8_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

data_chunk nonce_::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void nonce_::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void nonce_::to_data(writer& sink) const
{
    sink.write_8_bytes_little_endian(nonce);
}

uint64_t nonce_::serialized_size() const
{
    return nonce_::satoshi_fixed_size();
}

uint64_t nonce_::satoshi_fixed_size()
{
    return 8;
}

bool operator==(const nonce_& left, const nonce_& right)
{
    return (left.nonce == right.nonce);
}

bool operator!=(const nonce_& left, const nonce_& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
