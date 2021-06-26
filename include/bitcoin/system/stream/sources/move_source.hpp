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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SOURCES_MOVE_SOURCE_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SOURCES_MOVE_SOURCE_HPP

#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/stream/device.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Source for boost::iostreams::stream, moves bytes from Container.
template <typename Container, if_base_of<data_slab, Container> = true>
class move_source
  : public device<Container>
{
public:
    typedef Container container;
    struct category
      : boost::iostreams::source_tag
    {
    };

    move_source(Container data) noexcept
      : device(limit<size_type>(data.size())),
        container_(data),
        next_(data.begin())
    {
    }

protected:
    void do_read(value_type* to, size_type size) noexcept override
    {
        // std::move returns iterator past last element moved to.
        auto end = std::next(next_, size);
        std::move(next_, end, to);
        next_ = end;
    }

private:
    const Container container_;
    typename Container::iterator next_;
};

} // namespace system
} // namespace libbitcoin

#endif
