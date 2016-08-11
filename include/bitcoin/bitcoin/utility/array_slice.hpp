/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_ARRAY_SLICE_HPP
#define LIBBITCOIN_ARRAY_SLICE_HPP

#include <cstddef>

namespace libbitcoin {

/**
 * An read-only STL-style wrapper for array-style collections.
 *
 * This class allows std::array, std::vector, std::string, and c-style arrays
 * to be used interchangeably in functions that expect raw data.
 */
template <typename T>
class array_slice
{
public:
    template <typename Container>
    array_slice(const Container& container);

    // TODO: reliance this class causes unnecessary copying in cases where move
    // arguments are not defined. This cannot itself be converted to move.
    ////template <typename Container>
    ////array_slice(Container&& container);

    array_slice(const T* begin, const T* end);

    const T* begin() const;
    const T* end() const;
    const T* data() const;
    std::size_t size() const;
    bool empty() const;

private:
    const T* begin_;
    const T* end_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/array_slice.ipp>

#endif

