/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_ARRAY_SLICE_HPP
#define LIBBITCOIN__UTILITY_ARRAY_SLICE_HPP

#include <bitcoin/bitcoin/utility/array_slice.hpp>

namespace libbitcoin {
namespace api {

/**
 * An read-only STL-style wrapper for array-style collections.
 *
 * This class allows std::array, std::vector, std::string, and c-style arrays
 * to be used interchangeably in functions that expect an iterable container.
 * This is not a substitute for move overloads. Consider also that a contiguous
 * buffer (c-style array) is more performant than the iterator abstraction.
 */
template <typename Iterable>
class utility_array_slice
{
public:
    template <typename Container>
    utility_array_slice(const Container& container);

    utility_array_slice(const Iterable* begin, const Iterable* end);

    const Iterable* begin() const;
    const Iterable* end() const;
    const Iterable* data() const;
    std::size_t size() const;
    bool empty() const;

    libbitcoin::array_slice getValue() {
        return value;
    }

    void setValue(libbitcoin::array_slice value) {
        this->value = value;
    }
private:
    libbitcoin::array_slice value;

//private:
//    const Iterable* begin_;
//    const Iterable* end_;
};

} // namespace api
} // namespace libbitcoin

//#include <bitcoin/bitcoin/impl/utility/array_slice.ipp>

#endif

