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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_SINK_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_SINK_HPP

#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Virtual base class for boost::iostreams::stream sinks.
template <typename Container>
class base_sink
{
public:
    typedef char char_type;
    typedef std::streamsize size_type;
    typedef boost::iostreams::sink_tag category;
    typedef typename Container::value_type value_type;

    size_type write(const char_type* buffer, size_type count) noexcept;

protected:
    base_sink(size_type size) noexcept;
    virtual void do_write(const value_type* from, size_type size) noexcept = 0;

    // Size tracks the Container space remaining.
    size_type size_;
};

/// Alias for defining an ostream from a container and a sink template.
template <typename Container, template <class = Container> class Sink,
    if_base_of<base_sink<Container>, Sink<Container>> = true>
using sink = boost::iostreams::stream<Sink<Container>>;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/sinks/sink.ipp>

#endif
