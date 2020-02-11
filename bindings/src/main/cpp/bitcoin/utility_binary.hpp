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
#ifndef LIBBITCOIN__UTILITY_BINARY_HPP
#define LIBBITCOIN__UTILITY_BINARY_HPP

#include <cstdint>
#include <string>
//#include <bitcoin/bitcoin/constants.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/p_std_size_t.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/utility_data_chunk.hpp>
#include <bitcoin/utility_data_slice.hpp>
#include <bitcoin/utility_size_type.hpp>

namespace libbitcoin {
namespace api {

class BC_API utility_binary
{
public:
    typedef uint8_t block;
//    typedef libbitcoin::api::utility_size_type size_type;

//    typedef p_std_size_t size_type;
    typedef std::size_t size_type;

    static BC_CONSTEXPR size_type bits_per_block = byte_bits;
    static size_type blocks_size(size_type bit_size);
    static bool is_base2(const std::string& text);

    utility_binary();
    utility_binary(const utility_binary& other);
    utility_binary(const std::string& bit_string);
    utility_binary(size_type size, uint32_t number);
    utility_binary(size_type size, utility_data_slice blocks);

    void resize(size_type size);
//    bool operator[](size_type index) const;
    bool elementAt(size_type index) const;
    const utility_data_chunk& blocks() const;
    std::string encoded() const;

    /// size in bits
    size_type size() const;
    void append(const utility_binary& post);
    void prepend(const utility_binary& prior);
    void shift_left(size_type distance);
    void shift_right(size_type distance);
    utility_binary substring(size_type first, size_type length=max_size_t) const;

    bool is_prefix_of(utility_data_slice field) const;
    bool is_prefix_of(uint32_t field) const;
    bool is_prefix_of(const utility_binary& field) const;

//    bool operator<(const utility_binary& other) const;
    bool le(const utility_binary& other) const;
//    bool operator==(const utility_binary& other) const;
    bool eq(const utility_binary& other) const;
//    bool operator!=(const utility_binary& other) const;
//    utility_binary& operator=(const utility_binary& other);
    utility_binary& assign(const utility_binary& other);
//    friend std::istream& operator>>(std::istream& in, utility_binary& to);
//    friend std::ostream& operator<<(std::ostream& out, const utility_binary& of);

    libbitcoin::binary getValue() {
        return value;
    }

    void setValue(libbitcoin::binary value) {
        this->value = value;
    }

private:
    libbitcoin::binary value;
//    static uint8_t shift_block_right(uint8_t next, uint8_t current, uint8_t prior,
//        size_type original_offset, size_type intended_offset);
//
//    data_chunk blocks_;
//    uint8_t final_block_excess_;
};

} // namespace api
} // namespace libbitcoin

//namespace std
//{

//template<>
//struct hash<bc::utility_binary>
//{
//    size_t operator()(const bc::utility_binary& value) const
//    {
//        return std::hash<std::string>()(value.encoded());
//    }
//};

//} // namespace std

#endif
