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
#ifndef LIBBITCOIN_DATA_STREAM_HOST_HPP
#define LIBBITCOIN_DATA_STREAM_HOST_HPP

#include <istream>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data_buffer.hpp>

namespace libbitcoin {

template<typename SourceType, typename CharType>
class BC_API data_stream_host
{
private:

    data_buffer<SourceType, CharType> buffer_;

public:

    data_stream_host(const std::vector<SourceType>& data)
        : buffer_(const_cast<SourceType*>(data.data()), data.size()),
          stream(&buffer_)
    {
        BOOST_STATIC_ASSERT((sizeof(SourceType) == sizeof(CharType)));
    }

    std::basic_istream<CharType, std::char_traits<CharType>> stream;
};

typedef data_stream_host<uint8_t, char> data_chunk_stream_host;

} // namespace libbitcoin

#endif

