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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_WRITERS_WRITERS_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_WRITERS_WRITERS_HPP

#include <string>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/iostream/sinks/sink.hpp>
#include <bitcoin/system/iostream/sinks/copy_sink.hpp>
#include <bitcoin/system/iostream/sinks/push_sink.hpp>
#include <bitcoin/system/iostream/writers/bit_writer.hpp>
#include <bitcoin/system/iostream/writers/byte_writer.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Construct a member ostream and feed it to a writer.
template <typename Container, template <class = Container> class Sink,
    typename Writer, if_base_of<byte_writer, Writer> = true,
    if_base_of<base_sink<Container>, Sink<Container>> = true>
class make_writer
  : public Writer
{
public:
    make_writer(Container& sink) noexcept
      : sink_(sink), Writer(sink_) {}
    ~make_writer() noexcept override { Writer::~Writer(); }

private:
    sink<Container, Sink> sink_;
};

namespace write
{
    namespace bytes
    {
        // write::bytes::copy
        using copy = make_writer<data_slab, copy_sink, byte_writer>;
        using push = make_writer<data_chunk, push_sink, byte_writer>;
        using text = make_writer<std::string, copy_sink, byte_writer>;
    }

    namespace bits
    {
        // write::bits::copy
        using copy = make_writer<data_slab, copy_sink, bit_writer>;
        using push = make_writer<data_chunk, push_sink, bit_writer>;
        using text = make_writer<std::string, copy_sink, bit_writer>;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
