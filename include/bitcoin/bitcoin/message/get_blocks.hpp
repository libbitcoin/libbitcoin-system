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
#ifndef LIBBITCOIN_MESSAGE_GET_BLOCKS_HPP
#define LIBBITCOIN_MESSAGE_GET_BLOCKS_HPP

#include <istream>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

typedef std::vector<hash_digest> block_locator;

class BC_API get_blocks
{
public:

    static const std::string satoshi_command;

    get_blocks();

    get_blocks(const block_locator& start_hashes, const hash_digest& hash_stop);

    get_blocks(std::istream& stream);

    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator& start_hashes();

    const block_locator& start_hashes() const;

    void start_hashes(const block_locator& value);

    hash_digest& hash_stop();

    const hash_digest& hash_stop() const;

    void hash_stop(const hash_digest& value);

    data_chunk to_data() const;

    size_t satoshi_size() const;

private:

    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator start_hashes_;
    hash_digest hash_stop_;
};

} // end message
} // end libbitcoin

#endif
