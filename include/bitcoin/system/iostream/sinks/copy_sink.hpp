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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_COPY_SINK_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_COPY_SINK_HPP

#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/iostream/sinks/sink.hpp>

namespace libbitcoin {
namespace system {

/// Sink for boost::iostreams::stream, copies bytes to Container.
template <typename Container>
class copy_sink
  : public base_sink<sink::tag::copy, Container>
{
public:
    copy_sink(Container& data) noexcept;

    /// detail::random_access direct (seekable)
    sequence output_sequence() noexcept;

protected:
    virtual void do_write(const value_type* from, size_type size) noexcept;

private:
    const typename Container& container_;
    typename Container::iterator to_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/sinks/copy_sink.ipp>

#endif
