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
#include <bitcoin/system/iostream/data/bit_reader.hpp>
#include <bitcoin/system/iostream/data/bit_writer.hpp>
#include <bitcoin/system/iostream/data/byte_reader.hpp>
#include <bitcoin/system/iostream/data/byte_writer.hpp>
#include <bitcoin/system/iostream/streams/base_sink.hpp>
#include <bitcoin/system/iostream/streams/base_source.hpp>
#include <bitcoin/system/iostream/streams/copy_sink.hpp>
#include <bitcoin/system/iostream/streams/push_sink.hpp>
#include <bitcoin/system/iostream/streams/copy_source.hpp>
#include <bitcoin/system/iostream/streams/move_source.hpp>

namespace libbitcoin {
namespace system {

// Transitional aliases (formerly interfaces now base classes).
using reader = byte_reader;
using writer = byte_writer;

/// Input steams.

template <typename Container>
std::istream istream(const Container& container) noexcept
{
    // Copies bytes from the container.
    using namespace boost::iostreams;
    return stream<copy_source<Container>>(container);
}

template <typename Container>
std::istream istream(Container&& container) noexcept
{
    // Moves bytes from the container.
    using namespace boost::iostreams;
    return stream<move_source<Container>>(std::forward(container));
}

template <typename Container>
std::istream unchecked_istream(
    const typename Container::const_iterator& begin) noexcept
{
    // Copies bytes from the container, with unchecked end.
    using namespace boost::iostreams;
    typedef size_type = typename Container::size_type;
    constexpr auto maximum = std::numeric_limits<size_type>::max();
    return stream<copy_source<Container>>(begin, std::next(begin, maximum));
}

/// istream* and bit/byte reader construction examples.
////const data_chunk copyable(1024, 0x42);
////data_chunk moveable = copyable;
////bit_reader reader(copyable);
////bit_reader mover(std::move(moveable));
////byte_reader reader(copyable);
////byte_reader mover(std::move(moveable));
////byte_reader unchecked_reader(copyable.begin());

/// Output steams.

template <typename Container>
std::ostream ostream(Container& container) noexcept
{
    // Inserts bytes into the end of the container.
    using namespace boost::iostreams;
    return stream<push_sink<Container>>(container);
}

template <typename Container>
std::ostream ostream(typename Container::iterator& begin,
    typename Container::iterator& end) noexcept
{
    // Copies bytes to the container.
    using namespace boost::iostreams;
    return stream<copy_sink<Container>>(begin, end);
}

template <typename Container>
std::ostream unchecked_ostream(typename Container::iterator& begin) noexcept
{
    // Copies bytes to the container, with unchecked end.
    using namespace boost::iostreams;
    typedef size_type = typename Container::size_type;
    constexpr auto maximum = std::numeric_limits<size_type>::max();
    return stream<copy_sink<Container>>(begin, std::next(begin, maximum));
}

/// ostream* and bit/byte writer construction examples.
////data_chunk empty;
////data_chunk populated(1024, 0x42);
////bit_writer pusher(empty);
////bit_writer writer(populated.begin(), populated.end());
////byte_writer pusher(empty);
////byte_writer writer(populated.begin(), populated.end());
////byte_writer unchecked_writer(populated.begin());

} // namespace system
} // namespace libbitcoin

#endif
