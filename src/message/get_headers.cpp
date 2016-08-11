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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/get_headers.hpp>

#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/message/version.hpp>

namespace libbitcoin {
namespace message {

const std::string get_headers::command = "getheaders";
const uint32_t get_headers::version_minimum = version::level::headers;
const uint32_t get_headers::version_maximum = version::level::maximum;

get_headers get_headers::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    get_headers instance;
    instance.from_data(version, data);
    return instance;
}

get_headers get_headers::factory_from_data(uint32_t version,
    std::istream& stream)
{
    get_headers instance;
    instance.from_data(version, stream);
    return instance;
}

get_headers get_headers::factory_from_data(uint32_t version,
    reader& source)
{
    get_headers instance;
    instance.from_data(version, source);
    return instance;
}

get_headers::get_headers()
  : get_blocks()
{
}

get_headers::get_headers(const hash_list& start, const hash_digest& stop)
  : get_blocks(start, stop)
{
}

get_headers::get_headers(hash_list&& start, hash_digest&& stop)
  : get_headers(start, stop)
{
}

bool get_headers::from_data(uint32_t version, const data_chunk& data)
{
    return get_blocks::from_data(version, data);
}

bool get_headers::from_data(uint32_t version, std::istream& stream)
{
    return get_blocks::from_data(version, stream);
}

bool get_headers::from_data(uint32_t version, reader& source)
{
    bool result = !(version < version_minimum);

    if (result)
        result = get_blocks::from_data(version, source);
    else
        reset();

    return result;
}

} // end message
} // end libbitcoin
