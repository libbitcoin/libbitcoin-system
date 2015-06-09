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
#ifndef LIBBITCOIN_CONTAINER_SOURCE_HPP
#define LIBBITCOIN_CONTAINER_SOURCE_HPP

#include <algorithm>
//#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

// modified from boost.iostreams example
// http://www.boost.org/doc/libs/1_55_0/libs/iostreams/doc/tutorial/container_source.html
template<typename Container, typename SourceType, typename CharType>
class BC_API container_source
{
public:

    typedef CharType char_type;
    typedef boost::iostreams::source_tag category;

    container_source(const Container& container)
        : container_(container), pos_(0)
    {
        BOOST_STATIC_ASSERT((sizeof(SourceType) == sizeof(CharType)));
    }

    std::streamsize read(char_type* s, std::streamsize n)
    {
        auto amt = static_cast<std::streamsize>(container_.size() - pos_);
        auto result = std::min(n, amt);

        if (result > 0)
        {
            BITCOIN_ASSERT(std::numeric_limits<typename Container::size_type>::max()
                >= std::numeric_limits<std::streamsize>::max());

            auto upperbound = pos_ + static_cast<typename Container::size_type>(result);

            std::copy(
                container_.begin() + pos_,
                container_.begin() + upperbound,
                s);

            pos_ = upperbound;
        }
        else
        {
            result = -1; // EOF
        }

        return result;
    }

private:

    const Container& container_;
    typename Container::size_type pos_;
};

template<typename Container>
using byte_source = container_source<Container, uint8_t, char>;

} // namespace libbitcoin

#endif

