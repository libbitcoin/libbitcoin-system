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
#ifndef LIBBITCOIN_STEALTH_HPP
#define LIBBITCOIN_STEALTH_HPP

#include <bitcoin/bitcoin/types.hpp>

namespace libbitcoin {

typedef uint32_t stealth_bitfield;

class BC_API stealth_prefix
{
public:
    static constexpr size_t bits_per_block = byte_bits;

    stealth_prefix();
    stealth_prefix(const std::string& bitstring);
    stealth_prefix(size_t size, uint32_t value);
    stealth_prefix(size_t size, const data_chunk& blocks);

    void resize(size_t size);

    bool operator[](size_t i) const;

    uint32_t uint32() const;
    const data_chunk& blocks() const;

    size_t size() const;

private:
    friend bool operator==(
        const stealth_prefix& prefix_a, const stealth_prefix& prefix_b);
    friend std::ostream& operator<<(
        std::ostream& stream, const stealth_prefix& prefix);

    size_t size_;
    data_chunk blocks_;
};

BC_API bool operator==(
    const stealth_prefix& prefix_a, const stealth_prefix& prefix_b);
BC_API std::ostream& operator<<(
    std::ostream& stream, const stealth_prefix& prefix);

BC_API bool match(const data_chunk& bytes, const stealth_prefix& prefix);

BC_API stealth_bitfield calculate_stealth_bitfield(
    const data_chunk& stealth_data);

} // namespace libbitcoin

#endif

