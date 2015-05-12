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
#ifndef LIBBITCOIN_DATA_BUFFER_HPP
#define LIBBITCOIN_DATA_BUFFER_HPP

#include <istream>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

template<typename SourceType, typename CharType>
class BC_API data_buffer : public std::basic_streambuf<CharType, std::char_traits<CharType>>
{
public:

    data_buffer(SourceType* data, size_t size)
    {
        BOOST_STATIC_ASSERT((sizeof(SourceType) == sizeof(CharType)));

       CharType* start =
            reinterpret_cast<CharType*>(data);
        CharType* finish = start + size;
        CharType* current = start;

        std::basic_streambuf<CharType, std::char_traits<CharType>>::setg(start, current, finish);
    }
};

typedef data_buffer<uint8_t, char> data_chunk_buffer;

} // namespace libbitcoin

#endif

