/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_HPP
#define LIBBITCOIN_SYSTEM_TEST_HPP

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <ostream>
#include <sstream>
#include <vector>
#include <bitcoin/system.hpp>

using namespace bc;
using namespace bc::system;

namespace std {

// data_chunk/byte_array -> base16(data)
std::ostream& operator<<(std::ostream& stream,
    const data_slice& slice) noexcept;

// vector<Type> -> join(<<Type)
template <typename Type>
std::ostream& operator<<(std::ostream& stream,
    const std::vector<Type>& values) noexcept
{
    // Ok when testing serialize because only used for error message out.
    stream << serialize(values);
    return stream;
}

// array<Type, Size> -> join(<<Type)
template <typename Type, size_t Size>
std::ostream& operator<<(std::ostream& stream,
    const std::array<Type, Size>& values) noexcept
{
    // Ok when testing serialize because only used for error message out.
    stream << serialize(values);
    return stream;
}

} // namespace std

#endif
