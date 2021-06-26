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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SINKS_PUSH_SINK_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SINKS_PUSH_SINK_HPP

#include <iterator>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/addition.hpp>
#include <bitcoin/system/math/sign.hpp>
#include <bitcoin/system/stream/device.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Sink for boost::iostreams::stream, appends bytes to Container.
/// Container may be any insertable object with contiguous byte data.
/// This is limited to std::string and std::vector of int8_t/uint8_t.
/// Push streams are buffered, indirect (inefficient) and require flush.
template <typename Container, if_byte_insertable<Container> = true>
class push_sink
  : public device<Container>
{
public:
    typedef Container& container;
    struct category
      : boost::iostreams::sink_tag,
        boost::iostreams::optimally_buffered_tag
    {
    };

    push_sink(Container& data) noexcept
      : device(limit<size_type>(data.max_size() - data.size())),
        container_(data),
        next_(data.begin())
    {
    }

protected:
    const size_type minimum_buffer_size = 1024;

    void do_write(const value_type* from, size_type size) noexcept override
    {
        auto start = container_.insert(next_, from, std::next(from, size));
        next_ = std::next(start, size);
    }

    size_type do_optimal_buffer_size() const noexcept override
    {
        // This is only called at stream construct.
        auto space = floored_subtract(container_.capacity(), container_.size());
        return greater<size_type>(space, minimum_buffer_size);
    }

protected:
    Container& container_;
    typename Container::iterator next_;
};

} // namespace system
} // namespace libbitcoin

#endif
