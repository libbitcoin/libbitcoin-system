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
#ifndef LIBBITCOIN_DATA_SOURCE_HPP
#define LIBBITCOIN_DATA_SOURCE_HPP

#include <algorithm>
//#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>

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

        if (result != 0)
        {
            std::copy(
                container_.begin() + pos_,
                container_.begin() + pos_ + result,
                s);

            pos_ += result;
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

