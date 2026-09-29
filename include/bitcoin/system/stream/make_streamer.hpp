/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_MAKE_STREAMER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_MAKE_STREAMER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/make_stream.hpp>

// Suppress multiple inheritance warnings.
// The inheritance is virtual, so not actually multiple.
// But the boost type constraint 'is_virtual_base_of' triggers the warning.
BC_PUSH_WARNING(DIAMOND_INHERITANCE)

namespace libbitcoin {
namespace system {

/// Base stream owner, destroyed after the streamer (which flushes on destruct).
template <typename Stream>
class stream_owner
{
protected:
    template <typename Container>
    stream_owner(Container&& device) NOEXCEPT
      : owned_(std::forward<Container>(device))
    {
    }

    Stream owned_;
};

/// Construct a stream and feed it to a streamer.
template <typename Device,
    template <typename> class Base,
    typename Stream = make_stream<Device>,
    typename Streamer = Base<Stream>>
class make_streamer
  : private stream_owner<Stream>,
    public Streamer
{
public:
    using ptr = std::shared_ptr<make_streamer<Device, Base, Stream, Streamer>>;

    make_streamer(typename Device::container device) NOEXCEPT
      : stream_owner<Stream>(device), Streamer()
    {
        Streamer::set_stream(&this->owned_);
    }
};

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()

#endif
