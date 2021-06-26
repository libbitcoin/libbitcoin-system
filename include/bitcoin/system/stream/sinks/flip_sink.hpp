/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SINKS_FLIP_SINK_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SINKS_FLIP_SINK_HPP

#include <algorithm>
#include <iterator>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/stream/sinks/copy_sink.hpp>

namespace libbitcoin {
namespace system {

/// Sink for boost::iostreams::stream, copies bytes to/from Container.
template <typename Container>
class flip_sink
  : public copy_sink<Container>
{
public:
    struct category
      : boost::iostreams::seekable,
        boost::iostreams::direct_tag
    {
        // The only derivation is override of copy_sink::category.
    };

    flip_sink(const Container& data) noexcept
      : copy_sink(data)
    {
    }

protected:
    ////void do_read(value_type* to, size_type size) noexcept override
    ////{
    ////    // std::copy_n returns iterator past last element copied to.
    ////    std::copy_n(next_, size, to);
    ////    std::advance(next_, size);
    ////}
};

} // namespace system
} // namespace libbitcoin

#endif
