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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/stream/binary.hpp>
#include <bitcoin/system/stream/device.hpp>
#include <bitcoin/system/stream/devices/copy_sink.hpp>
#include <bitcoin/system/stream/devices/copy_source.hpp>
#include <bitcoin/system/stream/devices/flip_sink.hpp>
#include <bitcoin/system/stream/devices/push_sink.hpp>
#include <bitcoin/system/stream/iostream/istream.hpp>
#include <bitcoin/system/stream/iostream/iostream.hpp>
#include <bitcoin/system/stream/iostream/ostream.hpp>
#include <bitcoin/system/stream/make_streamer.hpp>
#include <bitcoin/system/stream/streamers/bit_flipper.hpp>
#include <bitcoin/system/stream/streamers/bit_reader.hpp>
#include <bitcoin/system/stream/streamers/bit_writer.hpp>
#include <bitcoin/system/stream/streamers/byte_flipper.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bitflipper.hpp>
#include <bitcoin/system/stream/streamers/interfaces/byteflipper.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bitreader.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytereader.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bitwriter.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bytewriter.hpp>
#include <bitcoin/system/stream/streamers/sha256_writer.hpp>
#include <bitcoin/system/stream/streamers/sha256x2_writer.hpp>

// Stream Exceptions:
// ============================================================================
// "All exceptions thrown are propogated by the public member functions, so
// users of raw stream buffers must in principle be prepared to handle
// exceptions. However, the stream buffers which ship with most standard
// library implementations do not throw exceptions."
// www.boost.org/doc/libs/1_78_0/libs/iostreams/doc/guide/exceptions.html
// If a streamer is constructed with a throwing stream, an abort will likely
// occur upon usage. A stream may be configured to throw, see std::basic_ios:
// "Sets the exception mask to except. If the stream has an error state covered
// by the exception mask when called, an exception is immediately triggered."
// en.cppreference.com/w/cpp/io/basic_ios/exceptions
// Streams may be tested for .exceptions() == std::iostream::goodbit, but this
// cannot be unset once set. So caller must use non-throwing stream.
// ============================================================================

namespace libbitcoin {
namespace system {

namespace read
{
    namespace bytes
    {
        /// A byte reader that reads from a std::istream.
        using istream = byte_reader<std::istream>;

        /// A fast byte reader that reads from a system::istream.
        using fast = byte_reader<system::istream<>>;

        /// A byte reader that copies from a data_reference via std::istream.
        using copy = make_streamer<copy_source<data_reference>, byte_reader>;
    }

    namespace bits
    {
        /// A bit reader that reads from a std::istream.
        using istream = bit_reader<std::istream>;

        /// A fast bit reader that reads from a system::istream.
        using fast = bit_reader<system::istream<>>;

        /// A bit reader that copies from a data_reference via std::istream.
        using copy = make_streamer<copy_source<data_reference>, bit_reader>;
    }
}

namespace write
{
    namespace bytes
    {
        /// A byte writer that writes to a std::ostream.
        using ostream = byte_writer<std::ostream>;

        /// A fast byte writer that writes to a system::ostream.
        using fast = byte_writer<system::ostream<>>;

        /// A byte writer that copies to a data_slab via std::ostream.
        using copy = make_streamer<copy_sink<data_slab>, byte_writer>;

        /// A byte writer that inserts into a container via std::ostream.
        template <typename Container>
        using push = make_streamer<push_sink<Container>, byte_writer>;
        using text = push<std::string>;
        using data = push<data_chunk>;
    }

    namespace bits
    {
        /// A bit writer that writes to a std::ostream.
        using ostream = bit_writer<std::ostream>;

        /// A fast bit writer that writes to a std::ostream.
        using fast = bit_writer<system::ostream<>>;

        /// A bit writer that copies to a data_slab.
        using copy = make_streamer<copy_sink<data_slab>, bit_writer>;

        /// A bit writer that inserts into a container via std::ostream.
        template <typename Container>
        using push = make_streamer<push_sink<Container>, bit_writer>;
        using text = push<std::string>;
        using data = push<data_chunk>;
    }
}

namespace flip
{
    namespace bytes
    {
        /// A byte reader/writer of a std::iostream.
        using iostream = byte_flipper<std::iostream>;

        /// A fast byte reader/writer of a system::iostream.
        using fast = byte_flipper<system::iostream<>>;

        /// A byte reader/writer of a data_slab (no push and requires own sink).
        using copy = make_streamer<flip_sink<data_slab>, byte_flipper>;
    }

    namespace bits
    {
        /// A bit reader/writer of a std::iostream.
        using iostream = bit_flipper<std::iostream>;

        /// A fast bit reader/writer of a system::iostream.
        using fast = bit_flipper<system::iostream<>>;

        /// A bit reader/writer of a data_slab (no push and requires own sink).
        using copy = make_streamer<flip_sink<data_slab>, bit_flipper>;
    }
}

namespace hash
{
    namespace sha256
    {
        /// A hash writer that writes a sha256 hash to a std::ostream.
        using ostream = sha256_writer<std::ostream>;

        /// A fast hash writer that writes a sha256 hash to a system::ostream.
        using fast = sha256_writer<system::ostream<>>;

        /// A hash writer that copies a sha256 hash to a data_slab.
        using copy = make_streamer<copy_sink<data_slab>, sha256_writer>;

        /// A hash writer that inserts a sha256 hash into a container.
        template <typename Container>
        using push = make_streamer<push_sink<Container>, sha256_writer>;
        using text = push<std::string>;
        using data = push<data_chunk>;
    }

    namespace sha256x2
    {
        /// A hash writer that writes a bitcoin hash to a std::ostream.
        using ostream = sha256x2_writer<std::ostream>;

        /// A fast hash writer that writes a bitcoin hash to a system::ostream.
        using fast = sha256x2_writer<system::ostream<>>;

        /// A hash writer that copies a bitcoin hash to a data_slab.
        using copy = make_streamer<copy_sink<data_slab>, sha256x2_writer>;

        /// A hash writer that inserts a bitcoin hash into a container.
        template <typename Container>
        using push = make_streamer<push_sink<Container>, sha256x2_writer>;
        using text = push<std::string>;
        using data = push<data_chunk>;
    }
}

/// Interface byte aliases.
using flipper = byteflipper;
using reader = bytereader;
using writer = bytewriter;

} // namespace system
} // namespace libbitcoin

#endif
