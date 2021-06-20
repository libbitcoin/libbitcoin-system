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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_PUSH_SINK_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_PUSH_SINK_HPP

#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/iostream/streams/base_sink.hpp>

namespace libbitcoin {
namespace system {

/// Sink for boost::iostreams::stream, appends bytes to Container.
template <typename Container>
class push_sink
  : public base_sink<Container>
{
public:
    push_sink() noexcept;
    virtual ~push_sink() noexcept;
    push_sink(Container& data) noexcept;

protected:
    virtual void do_write(const value_type* from, size_type size) noexcept;

private:
    Container& sink_;
    typename Container::iterator to_;
};

template <typename Container>
using ostream_push = boost::iostreams::stream<push_sink<Container>>;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/streams/push_sink.ipp>

// TODO: implement rational skip() behavior with insert_sink.
// TODO: this requires the sink to be "output seekable"
// boost.org/doc/libs/1_75_0/libs/iostreams/doc/index.html

#endif
