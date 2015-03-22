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

#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

class BC_API binary_type
{
public:

    typedef uint8_t block;
    typedef std::size_t size_type;

    static BC_CONSTEXPR size_type bits_per_block = byte_bits;

    static size_type blocks_size(const size_type bitsize);

    binary_type();
    binary_type(const std::string& bitstring);
    binary_type(size_type size, data_slice blocks);

    void resize(size_type size);

    bool operator[](size_type index) const;

    const data_chunk& blocks() const;

    // size in bits
    size_type size() const;

    void append(const binary_type& post);

    void prepend(const binary_type& prior);

    void shift_left(size_type distance);

    void shift_right(size_type distance);

    binary_type get_substring(size_type first, size_type length = SIZE_MAX) const;

private:
    friend bool operator==(
        const binary_type& prefix_a, const binary_type& prefix_b);
    friend std::istream& operator>>(
        std::istream& stream, binary_type& prefix);
    friend std::ostream& operator<<(
        std::ostream& stream, const binary_type& prefix);

    static uint8_t shift_block_right(uint8_t next, uint8_t current, uint8_t prior,
        size_type original_offset, size_type intended_offset);

    data_chunk blocks_;
    uint8_t final_block_excess_ = 0;
};

BC_API bool operator==(
    const binary_type& prefix_a, const binary_type& prefix_b);
BC_API bool operator!=(
    const binary_type& prefix_a, const binary_type& prefix_b);

BC_API std::istream& operator>>(
    std::istream& stream, binary_type& prefix);
BC_API std::ostream& operator<<(
    std::ostream& stream, const binary_type& prefix);

} // namespace libbitcoin

#endif


