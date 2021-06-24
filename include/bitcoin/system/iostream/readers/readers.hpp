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

// TODO: generalize to make_streamer and move to iostream root.

// Suppress multiple inheritance warnings.
// The inheritance is virtual, so not actually multiple.
// But the boost type constraint 'is_virtual_base_of' triggers the warning.
#pragma warning(push)
#pragma warning(disable : 4250)

/// Construct a member istream and feed it to a reader.
/// For std::istream reader, just pass to byte_reader or bit_reader instance.
template <typename Container,
    template <typename = Container> class Source,
    template <typename = istream<Container, Source>> class Base,
    typename IStream = istream<Container, Source>,
    typename Reader = Base<IStream>>
class make_reader
  : public Reader
{
public:
    make_reader(const Container& source) noexcept
      : source_(source), Reader(source_)
    {
    }

    ~make_reader() noexcept override
    {
        Reader::~Reader();
    }

private:
    IStream source_;
};

#pragma warning(pop)

namespace read
{
    namespace bytes
    {
        using stream = byte_reader<std::istream>;
        using move = make_reader<data_slab, move_source, byte_reader>;
        using copy = make_reader<data_slice, copy_source, byte_reader>;
        using text = make_reader<std::string, copy_source, byte_reader>;
    }

    namespace bits
    {
        using stream = bit_reader<std::istream>;
        using move = make_reader<data_slab, move_source, bit_reader>;
        using copy = make_reader<data_slice, copy_source, bit_reader>;
        using text = make_reader<std::string, copy_source, bit_reader>;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
