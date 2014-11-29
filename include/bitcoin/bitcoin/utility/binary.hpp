/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_BINARY_HPP
#define LIBBITCOIN_BINARY_HPP

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/types.hpp>

namespace libbitcoin {

class BC_API binary_type
{
public:
    static constexpr size_t bits_per_block = byte_bits;

    static size_t blocks_size(const size_t bitsize);

    binary_type();
    binary_type(const std::string& bitstring);
    binary_type(size_t size, const data_slice& blocks);

    void resize(size_t size);

    bool operator[](size_t i) const;

    const data_chunk& blocks() const;

    /// size in bits
    size_t size() const;

private:
    friend bool operator==(
        const binary_type& prefix_a, const binary_type& prefix_b);
    friend std::ostream& operator<<(
        std::ostream& stream, const binary_type& prefix);

    size_t size_ = 0;
    data_chunk blocks_;
};

BC_API bool operator==(
    const binary_type& prefix_a, const binary_type& prefix_b);
BC_API bool operator!=(
    const binary_type& prefix_a, const binary_type& prefix_b);

BC_API std::ostream& operator<<(
    std::ostream& stream, const binary_type& prefix);

} // namespace libbitcoin

#endif


