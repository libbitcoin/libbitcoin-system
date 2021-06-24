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
/// For std::istream writer, just pass to byte_writer or bit_writer instance.
template <typename Container,
    template <typename = Container> class Sink,
    template <typename = ostream<Container, Sink>> class Writer,
    typename OStream = ostream<Container, Sink>>
class make_writer
  : public Writer<OStream>
{
public:
    make_writer(Container& sink) noexcept
      : sink_(sink), Writer(sink_)
    {
    }

    ~make_writer() noexcept override
    {
        Writer::~Writer();
    }

private:
    OStream sink_;
};

namespace write
{
    namespace bytes
    {
        using stream = byte_writer<std::ostream>;
        using copy = make_writer<data_slab, copy_sink, byte_writer>;
        using push = make_writer<data_chunk, push_sink, byte_writer>;
        using text = make_writer<std::string, push_sink, byte_writer>;
    }

    namespace bits
    {
        using stream = bit_writer<std::ostream>;
        using copy = make_writer<data_slab, copy_sink, bit_writer>;
        using push = make_writer<data_chunk, push_sink, bit_writer>;
        using text = make_writer<std::string, push_sink, bit_writer>;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
