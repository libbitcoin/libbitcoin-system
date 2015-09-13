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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_PRIVATE_KEY_HPP
#define LIBBITCOIN_PARSE_ENCRYPTED_PRIVATE_KEY_HPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include "parse_encrypted_key.hpp"

namespace libbitcoin {
namespace wallet {

static constexpr size_t half = hash_size / 2;
static constexpr size_t quarter = half / 2;

typedef byte_array<half> half_hash;
typedef byte_array<quarter> quarter_hash;

// The above sizes are all tied to aes256.
static_assert(2 * quarter == bc::aes256_block_size, "oops!");

class parse_encrypted_private_key
  : public parse_encrypted_key<0x01, 2>
{
public:
    static byte_array<prefix_size> prefix(uint8_t address, bool multiplied);

    parse_encrypted_private_key(const private_key& key);

    uint8_t address_version() const;
    quarter_hash data1() const;
    half_hash data2() const;
    bool multiplied() const;

private:
    bool verify_context() const;
    bool verify_flags() const;

    static constexpr uint8_t default_context = 0x42;
    static constexpr uint8_t multiplied_context = 0x43;

    const quarter_hash data1_;
    const half_hash data2_;
};
} // namespace wallet
} // namespace libbitcoin

#endif
