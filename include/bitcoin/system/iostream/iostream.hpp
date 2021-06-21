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

#include <string>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/data/data.hpp>

/// Sinks.
#include <bitcoin/system/iostream/sinks/sink.hpp>
#include <bitcoin/system/iostream/sinks/copy_sink.hpp>
#include <bitcoin/system/iostream/sinks/push_sink.hpp>

/// Sources.
#include <bitcoin/system/iostream/sources/source.hpp>
#include <bitcoin/system/iostream/sources/copy_source.hpp>
#include <bitcoin/system/iostream/sources/move_source.hpp>

/// Readers.
#include <bitcoin/system/iostream/readers/byte_reader.hpp>
#include <bitcoin/system/iostream/readers/bit_reader.hpp>
#include <bitcoin/system/iostream/readers/readers.hpp>

/// Writers.
#include <bitcoin/system/iostream/writers/byte_writer.hpp>
#include <bitcoin/system/iostream/writers/bit_writer.hpp>
#include <bitcoin/system/iostream/writers/writers.hpp>

namespace libbitcoin {
namespace system {
namespace stream
{
    namespace in
    {
        // stream::in::move
        using move = source<data_slab, move_source>;
        using copy = source<data_slice, copy_source>;
        using text = source<std::string, copy_source>;
    }

    namespace out
    {
        // stream::out::copy
        using copy = sink<data_slab, copy_sink>;
        using push = sink<data_chunk, push_sink>;
        using text = sink<std::string, copy_sink>;
    }
}

/// Base class aliases used as interfaces.
using reader = byte_reader;
using writer = byte_writer;

/// Original names.
////using data_source = stream::in::copy;
////using data_sink = stream::out::push;

} // namespace system
} // namespace libbitcoin

#endif
