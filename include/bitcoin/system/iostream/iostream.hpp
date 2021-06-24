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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_IOSTREAM_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_IOSTREAM_HPP

#include <iostream>
#include <string>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/data/data.hpp>

/// Sinks.
#include <bitcoin/system/iostream/sinks/copy_sink.hpp>
#include <bitcoin/system/iostream/sinks/push_sink.hpp>
#include <bitcoin/system/iostream/sinks/sink.hpp>

/// Sources.
#include <bitcoin/system/iostream/sources/copy_source.hpp>
#include <bitcoin/system/iostream/sources/move_source.hpp>
#include <bitcoin/system/iostream/sources/source.hpp>

/// Iterfaces.
#include <bitcoin/system/iostream/interfaces/bitreader.hpp>
#include <bitcoin/system/iostream/interfaces/bytereader.hpp>
#include <bitcoin/system/iostream/interfaces/bitwriter.hpp>
#include <bitcoin/system/iostream/interfaces/bytewriter.hpp>

/// Readers.
#include <bitcoin/system/iostream/readers/bit_reader.hpp>
#include <bitcoin/system/iostream/readers/byte_reader.hpp>
#include <bitcoin/system/iostream/readers/readers.hpp>

/// Writers.
#include <bitcoin/system/iostream/writers/bit_writer.hpp>
#include <bitcoin/system/iostream/writers/byte_writer.hpp>
#include <bitcoin/system/iostream/writers/writers.hpp>

namespace libbitcoin {
namespace system {
namespace stream
{
    namespace in
    {
        using move = istream<data_slab, move_source>;
        using copy = istream<data_slice, copy_source>;
        using text = istream<std::string, copy_source>;
    }

    namespace out
    {
        using copy = ostream<data_slab, copy_sink>;
        using push = ostream<data_chunk, push_sink>;
        using text = ostream<std::string, push_sink>;
    }
}

/// Interface byte aliases.
using reader = bytereader;
using writer = bytewriter;

/// Original names.
////using data_source = stream::in::copy;
////using data_sink = stream::out::push;

} // namespace system
} // namespace libbitcoin

#endif

// aggregate tags:
// std_io_tag      : localizable_tag
// source_tag      : input, device_tag
// sink_tag        : output, device_tag
// istream_tag     : std_io_tag, device_tag
// ostream_tag     : std_io_tag, device_tag
// input_seekable  : input, detail::random_access
// output_seekable : output, detail::random_access
//
// input|output:
// Involves a single sequence of characters, for input|output.
// device_tag:
// Default behavior, buffering can be overridden by direct_tag.
// direct_tag:
// Device which provides access to its controlled sequences memory
// regions rather than via a socket-like interface (non-buffering).
// closable_tag:
// Filter or device which receives notifications immediately before 
// stream is closed.
// localizable_tag:
// Filter or device that receives notifications when a stream or
// buffer which contains it has its locale set using imbue|pubimbue.
// detail::random_access:
// Seekable device, direct device seek requires input|output_sequence.
// peekable_tag
// Peekable device allows a character read from the input sequence to
// be put back.