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
#ifndef LIBBITCOIN_SYSTEM_STREAM_DEVICES_PUSH_SINK_HPP
#define LIBBITCOIN_SYSTEM_STREAM_DEVICES_PUSH_SINK_HPP

#include <iterator>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/device.hpp>

namespace libbitcoin {
namespace system {

/// Sink for ios::stream, appends bytes to Container.
/// Container may be any insertable object with contiguous byte data.
/// This is limited to std::string and std::vector of uint8_t.
/// Push streams are buffered, indirect (inefficient) and require flush.
template <typename Container, if_byte_insertable<Container> = true>
class push_sink
  : public device<Container>
{
public:
    typedef Container& container;
    struct category
      : ios::sink_tag, ios::optimally_buffered_tag
    {
    };

    push_sink(Container& data) noexcept
      : device<Container>(limit<typename device<Container>::size_type>(
          data.max_size() - data.size())),
        container_(data)
    {
    }
    
    push_sink(push_sink&&) = default;
    push_sink(const push_sink&) = default;
    push_sink& operator=(push_sink&&) = default;
    push_sink& operator=(const push_sink&) = default;
    ~push_sink() override = default;

protected:
    const typename device<Container>::size_type default_buffer_size = 1024;

    void do_write(const typename device<Container>::value_type* from,
        typename device<Container>::size_type size) noexcept override
    {
        container_.insert(container_.end(), from, std::next(from, size));
    }

    typename device<Container>::size_type do_optimal_buffer_size()
        const noexcept override
    {
        // This is only called at stream construct.
        // The compiler determines capacity, so this may be unreliable to test.
        // Create a buffer equal to the reserved but unused space, or default.
        auto space = floored_subtract(container_.capacity(), container_.size());
        return is_zero(space) ? default_buffer_size : space;
    }

private:
    Container& container_;
};

} // namespace system
} // namespace libbitcoin

#endif
