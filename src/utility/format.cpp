/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/utility/format.hpp>

#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/base16.hpp>

namespace libbitcoin {

std::ostream& operator<<(std::ostream& stream, const data_chunk& data)
{
    stream << encode_base16(data);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash_digest& hash)
{
    stream << encode_base16(hash);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const short_hash& hash)
{
    stream << encode_base16(hash);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const point_type& point)
{
    stream << point.hash << ":" << point.index;
    return stream;
}

} // namespace libbitcoin

