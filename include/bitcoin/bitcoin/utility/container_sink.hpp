/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONTAINER_SINK_HPP
#define LIBBITCOIN_SYSTEM_CONTAINER_SINK_HPP

#include <algorithm>
#include <cstdint>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

// modified from boost.iostreams example
// boost.org/doc/libs/1_55_0/libs/iostreams/doc/tutorial/container_source.html
template <typename Container, typename SinkType, typename CharType>
class BC_API container_sink
{
public:
    typedef CharType char_type;
    typedef boost::iostreams::sink_tag category;

    container_sink(Container& container)
      : container_(container)
    {
        static_assert(sizeof(SinkType) == sizeof(CharType), "invalid size");
    }

    std::streamsize write(const char_type* buffer, std::streamsize size)
    {
        const auto safe_sink = reinterpret_cast<const SinkType*>(buffer);
        container_.insert(container_.end(), safe_sink, safe_sink + size);
        return size;
    }

private:
    Container& container_;
};

template <typename Container>
using byte_sink = container_sink<Container, uint8_t, char>;

using data_sink = boost::iostreams::stream<byte_sink<data_chunk>>;

} // namespace libbitcoin

#endif

