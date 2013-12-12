/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_BLOCK_HPP
#define LIBBITCOIN_BLOCK_HPP

#include <bitcoin/primitives.hpp>
#include <bitcoin/utility/big_number.hpp>

namespace libbitcoin {

enum class block_status
{
    orphan,
    confirmed,
    rejected
};

struct block_info
{
    block_status status;
    size_t height;
};

bool operator==(
    const block_header_type block_a, const block_header_type& block_b);

uint64_t block_value(size_t height);
big_number block_work(uint32_t bits);

hash_digest hash_block_header(const block_header_type& header);

index_list block_locator_indexes(int top_height);

block_type genesis_block();

} // namespace libbitcoin

#endif

