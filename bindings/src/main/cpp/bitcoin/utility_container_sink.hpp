/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN__CONTAINER_SINK_HPP
#define LIBBITCOIN__CONTAINER_SINK_HPP

//#include <algorithm>
//#include <cstdint>
//#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <utility_data_chunk.hpp>

namespace libbitcoin {
namespace api {

// modified from boost.iostreams example
// boost.org/doc/libs/1_55_0/libs/iostreams/doc/tutorial/container_source.html
template <typename Container, typename SinkType, typename CharType>
class BC_API utility_container_sink
{
public:
    typedef CharType char_type;
    typedef boost::iostreams::sink_tag category;

    utility_container_sink(Container& container)
      : value(new libbitcoin::container_sink(container))
    {
        static_assert(sizeof(SinkType) == sizeof(CharType), "invalid size");
    }

    std::streamsize write(const char_type* buffer, std::streamsize size)
    {
    	return value.write(buffer, size);
    }

public:
    libbitcoin::container_sink getValue() {
        return value;
    }

    void setValue(libbitcoin::container_sink value) {
        this->value = value;
    }
private:
    libbitcoin::container_sink value;
};

template <typename Container>
using utility_byte_sink = utility_container_sink<Container, uint8_t, char>;

using utility_data_sink = boost::iostreams::stream<utility_byte_sink<utility_data_chunk>>;

} // namespace api
} // namespace libbitcoin

#endif

