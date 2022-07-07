/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_DEVICES_COPY_SINK_HPP
#define LIBBITCOIN_SYSTEM_STREAM_DEVICES_COPY_SINK_HPP

#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/device.hpp>

namespace libbitcoin {
namespace system {

/// Sink for ios::stream, copies bytes to Container.
template <typename Container, if_base_of<data_slab, Container> = true>
class copy_sink
  : public device<Container>
{
public:
    typedef const Container& container;
    struct category
      : ios::output_seekable, ios::direct_tag
    {
    };

    copy_sink(const Container& data) NOEXCEPT
      : device<Container>(limit<typename device<Container>::size_type>(
          data.size())),
        container_(data),
        next_(data.begin())
    {
    }
    
    copy_sink(copy_sink&&) = default;
    copy_sink(const copy_sink&) = default;
    copy_sink& operator=(copy_sink&&) = default;
    copy_sink& operator=(const copy_sink&) = default;
    ~copy_sink() override = default;

protected:
    typename device<Container>::sequence do_sequence() const NOEXCEPT override
    {
        using char_type = typename device<Container>::char_type;

        BC_PUSH_WARNING(NO_REINTERPRET_CAST)
        return std::make_pair(
            reinterpret_cast<char_type*>(container_.begin()),
            reinterpret_cast<char_type*>(container_.end()));
        BC_POP_WARNING()
    }

private:
    const Container& container_;
    typename Container::iterator next_;
};

} // namespace system
} // namespace libbitcoin

#endif
