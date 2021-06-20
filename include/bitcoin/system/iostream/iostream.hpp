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
#include <iterator>
#include <limits>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/iostream/bit_reader.hpp>
#include <bitcoin/system/iostream/bit_writer.hpp>
#include <bitcoin/system/iostream/byte_reader.hpp>
#include <bitcoin/system/iostream/byte_writer.hpp>
#include <bitcoin/system/iostream/streams/base_sink.hpp>
#include <bitcoin/system/iostream/streams/base_source.hpp>
#include <bitcoin/system/iostream/streams/copy_sink.hpp>
#include <bitcoin/system/iostream/streams/push_sink.hpp>
#include <bitcoin/system/iostream/streams/copy_source.hpp>
#include <bitcoin/system/iostream/streams/move_source.hpp>

namespace libbitcoin {
namespace system {

/// Transitional aliases.

/// These were formerly interfaces and are now base classes.
using reader = byte_reader;
using writer = byte_writer;

/// These are the only sink/source in use presently.
/// The role of copy_sink was previously served by a specialized writer.
/// The role of copy_source was previously also served by redundant reader.
using data_sink = boost::iostreams::stream<push_sink<data_chunk>>;
using data_source = boost::iostreams::stream<copy_source<data_slice>>;

using icopy = boost::iostreams::stream<copy_source<data_slice>>;
using imove = boost::iostreams::stream<move_source<data_chunk>>;
using ocopy = boost::iostreams::stream<copy_sink<data_slice>>;
using opush = boost::iostreams::stream<push_sink<data_chunk>>;

} // namespace system
} // namespace libbitcoin

#endif
