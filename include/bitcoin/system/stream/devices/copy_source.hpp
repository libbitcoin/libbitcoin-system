/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_DEVICES_COPY_SOURCE_HPP
#define LIBBITCOIN_SYSTEM_STREAM_DEVICES_COPY_SOURCE_HPP

#include <utility>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/device.hpp>

namespace libbitcoin {
namespace system {

/// Source for ios::stream, copies bytes from Container.
template <typename Container, if_base_of<data_reference, Container> = true>
class copy_source
  : public device<Container>
{
public:
    typedef const Container& container;
    struct category
      : ios::input_seekable, ios::direct_tag
    {
    };

    copy_source(const Container& data) noexcept
      : device<Container>(limit<typename device<Container>::size_type>(
          data.size())),
        container_(data),
        next_(data.begin())
    {
    }
    
    copy_source(copy_source&&) = default;
    copy_source(const copy_source&) = default;
    copy_source& operator=(copy_source&&) = default;
    copy_source& operator=(const copy_source&) = default;
    ~copy_source() override = default;

protected:
    typename device<Container>::sequence do_sequence() const noexcept override
    {
        // boost input_sequence/output_sequence both require non-const buffer
        // ptrs, but the data member is const, so we must cast it for direct
        // devices. As a source the buffer should/must never be mutated.
        BC_PUSH_WARNING(NO_CONST_CAST)
        const auto begin = const_cast<typename device<Container>::value_type*>(
            container_.begin());
        const auto end = const_cast<typename device<Container>::value_type*>(
            container_.end());
        BC_POP_WARNING()

        return std::make_pair(
            integer_pointer_cast<typename device<Container>::char_type>(begin),
            integer_pointer_cast<typename device<Container>::char_type>(end));
    }

private:
    const Container& container_;
    typename Container::const_iterator next_;
};

} // namespace system
} // namespace libbitcoin

#endif
