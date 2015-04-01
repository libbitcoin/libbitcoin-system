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
#ifndef LIBBITCOIN_SATOSHI_PRIMITIVE_GET_BLOCKS_HPP
#define LIBBITCOIN_SATOSHI_PRIMITIVE_GET_BLOCKS_HPP

#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> block_locator_type;

class BC_API get_blocks_type
{
public:

    static const std::string satoshi_command;

    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator_type start_hashes;
    hash_digest hash_stop;

    get_blocks_type();

    get_blocks_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    get_blocks_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    get_blocks_type(const Iterator begin, const Iterator end);

    operator const data_chunk() const;

    size_t satoshi_size() const;

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

}

#include <bitcoin/bitcoin/impl/primitives/satoshi/get_blocks.ipp>

#endif
