/*
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
#ifndef LIBBITCOIN_CHAIN_BLOCK_HEADER_HPP
#define LIBBITCOIN_CHAIN_BLOCK_HEADER_HPP

#include <cstdint>
#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API block_header
{
public:

//    static block_header& from_data(bool& result, std::istream& stream);

    block_header();

//    block_header(const block_header& other) = delete;

    block_header(uint32_t version, hash_digest previous_block_hash,
        hash_digest merkle, uint32_t timestamp, uint32_t bits, uint32_t nonce);

    block_header(std::istream& stream);

    uint32_t version() const;

    void version(uint32_t version);

    hash_digest& previous_block_hash();

    const hash_digest& previous_block_hash() const;

    void previous_block_hash(const hash_digest& hash);

    hash_digest& merkle();

    const hash_digest& merkle() const;

    void merkle(const hash_digest& merkle);

    uint32_t timestamp() const;

    void timestamp(uint32_t timestamp);

    uint32_t bits() const;

    void bits(uint32_t bits);

    uint32_t nonce() const;

    void nonce(uint32_t nonce);

    data_chunk to_data() const;

//    data_chunk to_data() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

    hash_digest hash() const;

private:

    uint32_t version_;
    hash_digest previous_block_hash_;
    hash_digest merkle_;
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;
};

BC_API bool operator==(const block_header& block_a,
    const block_header& block_b);

} // end chain
} // end libbitcoin

#endif
