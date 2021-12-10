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
#ifndef LIBBITCOIN_SYSTEM_DATA_SET_HPP
#define LIBBITCOIN_SYSTEM_DATA_SET_HPP

#include <memory>
#include <vector>
#include <bitcoin/system/data/collection.hpp>

namespace libbitcoin {
namespace system {

/// Instantiations of this set may not have virtual destructors.
template<typename Element>
class set
  : public std::vector<Element>
{
public:
    /// For no-fill allocation of the set.
    static const default_allocator<Element> no_fill_allocator{};

    /// Use base class constructors.
    using std::vector<Element>::vector;
};

} // namespace system
} // namespace libbitcoin

#endif
