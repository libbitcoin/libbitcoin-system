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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHECKSUM_IPP
#define LIBBITCOIN_CHECKSUM_IPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

template <size_t Size>
bool build_checked_array(byte_array<Size>& out,
    std::initializer_list<data_slice> slices)
{
    return build_array(out, slices) && insert_checksum(out);
}

template<size_t Size>
bool insert_checksum(byte_array<Size>& out)
{
    if (out.size() < checksum_size)
        return false;

    data_chunk body(out.begin(), out.end() - checksum_size);
    const auto checksum = to_little_endian(bitcoin_checksum(body));
    std::copy(checksum.begin(), checksum.end(), out.end() - checksum_size);
    return true;
}

} // namespace libbitcoin

#endif
