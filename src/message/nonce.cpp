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

#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

nonce_base::nonce_base()
    : nonce_()
{
}

nonce_base::nonce_base(const uint64_t nonce)
    : nonce_(nonce)
{
}

nonce_base::nonce_base(const data_chunk& value)
: nonce_base(value.begin(), value.end())
{
}

uint64_t nonce_base::nonce() const
{
    return nonce_;
}

void nonce_base::nonce(uint64_t value)
{
    nonce_ = value;
}

nonce_base::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_8_bytes(nonce_);
    return result;
}

size_t nonce_base::satoshi_size() const
{
    return nonce_base::satoshi_fixed_size();
}

size_t nonce_base::satoshi_fixed_size()
{
    return 8;
}

ping::ping()
: nonce_base()
{
}

ping::ping(const data_chunk& value)
: nonce_base(value)
{
}

size_t ping::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

pong::pong()
: nonce_base()
{
}

pong::pong(const data_chunk& value)
: nonce_base(value)
{
}

size_t pong::satoshi_fixed_size()
{
    return nonce_base::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
