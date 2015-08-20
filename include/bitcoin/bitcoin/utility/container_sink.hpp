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
#ifndef LIBBITCOIN_CONTAINER_SINK_HPP
#define LIBBITCOIN_CONTAINER_SINK_HPP

#include <algorithm>
//#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

// modified from boost.iostreams example
// http://www.boost.org/doc/libs/1_55_0/libs/iostreams/doc/tutorial/container_source.html
template<typename Container, typename SinkType, typename CharType>
class BC_API container_sink
{
public:

    typedef CharType char_type;
    typedef boost::iostreams::sink_tag category;

    container_sink(Container& container)
        : container_(container)
    {
        BOOST_STATIC_ASSERT((sizeof(SinkType) == sizeof(CharType)));
    }

    std::streamsize write(const char_type* s, std::streamsize n)
    {
        const SinkType* safe_s = reinterpret_cast<const SinkType*>(s);
        container_.insert(container_.end(), safe_s, safe_s + n);
        return n;
    }

private:

    Container& container_;
};

template<typename Container>
using byte_sink = container_sink<Container, uint8_t, char>;

} // namespace libbitcoin

#endif

