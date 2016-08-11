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
#include <bitcoin/bitcoin/message/pong.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string pong::command = "pong";
const uint32_t pong::version_minimum = version::level::bip31;
const uint32_t pong::version_maximum = version::level::maximum;

pong pong::factory_from_data(uint32_t version, const data_chunk& data)
{
    pong instance;
    instance.from_data(version, data);
    return instance;
}

pong pong::factory_from_data(uint32_t version, std::istream& stream)
{
    pong instance;
    instance.from_data(version, stream);
    return instance;
}

pong pong::factory_from_data(uint32_t version, reader& source)
{
    pong instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t pong::satoshi_fixed_size(uint32_t version)
{
    return nonce_::satoshi_fixed_size(version);
}

pong::pong()
  : pong(0)
{
}

pong::pong(uint64_t nonce)
  : nonce_(nonce)
{
}

bool pong::from_data(uint32_t version, const data_chunk& data)
{
    return nonce_::from_data(version, data);
}

bool pong::from_data(uint32_t version, std::istream& stream)
{
    return nonce_::from_data(version, stream);
}

bool pong::from_data(uint32_t version, reader& source)
{
    bool result = !(version < pong::version_minimum);

    if (result)
        result = nonce_::from_data(version, source);

    if (!result)
        reset();

    return result;
}

} // namspace message
} // namspace libbitcoin
