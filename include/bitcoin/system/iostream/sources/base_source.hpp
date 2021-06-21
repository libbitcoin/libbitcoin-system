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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_BASE_SOURCE_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_BASE_SOURCE_HPP

#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Virtual base class for boost::iostreams::stream sources.
template <typename Container>
class base_source
{
public:
    typedef char char_type;
    typedef boost::iostreams::source_tag category;
    typedef std::streamsize size_type;
    typedef typename Container::value_type value_type;

    size_type read(char_type* buffer, size_type count) noexcept;

protected:
    base_source(size_type size) noexcept;
    virtual void do_read(value_type* to,  size_type size) noexcept = 0;

    // Size tracks the Container unread bytes remaining.
    size_type size_;
};

/// Alias for defining an istream from a container and a source template.
template <typename Container, template <class = Container> class Source,
    if_base_of<base_source<Container>, Source<Container>> = true>
using source = boost::iostreams::stream<Source<Container>>;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/sources/base_source.ipp>

#endif
