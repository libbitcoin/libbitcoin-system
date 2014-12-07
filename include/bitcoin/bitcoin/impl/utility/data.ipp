/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_DATA_IPP
#define LIBBITCOIN_DATA_IPP

namespace libbitcoin {

template<typename T>
data_chunk to_data_chunk(T iterable)
{
    return data_chunk(std::begin(iterable), std::end(iterable));
}

inline
data_chunk build_data(std::initializer_list<data_slice> slices,
    size_t extra_space)
{
    size_t size = 0;
    for (auto slice: slices)
        size += slice.size();

    data_chunk out;
    out.reserve(size + extra_space);
    for (auto slice: slices)
        out.insert(out.end(), slice.begin(), slice.end());

    return out;
}

template <typename D, typename T>
void extend_data(D& data, const T& other)
{
    data.insert(std::end(data), std::begin(other), std::end(other));
}

} // libbitcoin

#endif

