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
#ifndef LIBBITCOIN_CONTAINER_SOURCE_HPP
#define LIBBITCOIN_CONTAINER_SOURCE_HPP

#include <algorithm>
#include <cstdint>
#include <boost/iostreams/categories.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

// modified from boost.iostreams example
// boost.org/doc/libs/1_55_0/libs/iostreams/doc/tutorial/container_source.html
template <typename Container, typename SourceType, typename CharType>
class BC_API container_source
{
public:
    typedef CharType char_type;
    typedef boost::iostreams::source_tag category;

    container_source(const Container& container)
      : container_(container), position_(0)
    {
        static_assert(sizeof(SourceType) == sizeof(CharType), "invalid size");
    }

    std::streamsize read(char_type* buffer, std::streamsize size)
    {
        const auto amount = container_.size() - position_;
        const auto result = std::min(size,
            static_cast<std::streamsize>(amount));

        // TODO: use ios eof symbol (template-based).
        if (result <= 0)
            return -1;

        const auto value = static_cast<typename Container::size_type>(result);
        DEBUG_ONLY(const auto maximum = 
            std::numeric_limits<typename Container::size_type>::max());
        BITCOIN_ASSERT(value < maximum);
        BITCOIN_ASSERT(position_ + value < maximum);

        const auto limit = position_ + value;
        const auto start = container_.begin() + position_;
        const auto end = container_.begin() + limit;
        std::copy(start, end, buffer);
        position_ = limit;
        return result;
    }

private:
    const Container& container_;
    typename Container::size_type position_;
};

template <typename Container>
using byte_source = container_source<Container, uint8_t, char>;

using data_source = boost::iostreams::stream<byte_source<data_chunk>>;

} // namespace libbitcoin

#endif

