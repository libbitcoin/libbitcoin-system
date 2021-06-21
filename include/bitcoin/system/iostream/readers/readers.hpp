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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_READERS_READERS_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_READERS_READERS_HPP

#include <string>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/iostream/sources/source.hpp>
#include <bitcoin/system/iostream/sources/copy_source.hpp>
#include <bitcoin/system/iostream/sources/move_source.hpp>
#include <bitcoin/system/iostream/readers/bit_reader.hpp>
#include <bitcoin/system/iostream/readers/byte_reader.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Construct a member istream and feed it to a reader.
template <typename Container, template <class = Container> class Source,
    typename Reader, if_base_of<byte_reader, Reader> = true,
    if_base_of<base_source<Container>, Source<Container>> = true>
class make_reader
  : public Reader
{
public:
    make_reader(const Container& source) noexcept
      : source_(source), Reader(source_) {}
    ~make_reader() noexcept override { Reader::~Reader(); }

private:
    source<Container, Source> source_;
};

namespace read
{
    namespace bytes
    {
        // read::bytes::move
        using move = make_reader<data_slab, move_source, byte_reader>;
        using copy = make_reader<data_slice, copy_source, byte_reader>;
        using text = make_reader<std::string, copy_source, byte_reader>;
    }

    namespace bits
    {
        // read::bits::move
        using move = make_reader<data_slab, move_source, bit_reader>;
        using copy = make_reader<data_slice, copy_source, bit_reader>;
        using text = make_reader<std::string, copy_source, bit_reader>;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
