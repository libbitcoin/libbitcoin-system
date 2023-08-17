/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/make_stream.hpp>

// Suppress multiple inheritance warnings.
// The inheritance is virtual, so not actually multiple.
// But the boost type constraint 'is_virtual_base_of' triggers the warning.
BC_PUSH_WARNING(4250)

namespace libbitcoin {
namespace system {

/// Construct an output stream and feed it to a writer.
/// For std::ostream just pass to writer on construct.
template <typename Device,
    template <typename = make_stream<Device>> class Base,
    typename Stream = make_stream<Device>, typename Streamer = Base<Stream>>
class make_streamer
  : public Streamer
{
public:
    using ptr = std::shared_ptr<make_streamer<Device, Base, Stream, Streamer>>;

    make_streamer(typename Device::container device) NOEXCEPT
      : stream_(device), Streamer(stream_)
    {
    }

protected:
    Stream stream_;
};

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()

#endif
