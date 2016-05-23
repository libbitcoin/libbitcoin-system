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
#ifndef LIBBITCOIN_MESSAGE_GET_BLOCKS_HPP
#define LIBBITCOIN_MESSAGE_GET_BLOCKS_HPP

#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

typedef std::vector<hash_digest> block_locator;

class BC_API get_blocks
{
public:
    typedef std::shared_ptr<get_blocks> ptr;

    static get_blocks factory_from_data(const data_chunk& data);
    static get_blocks factory_from_data(std::istream& stream);
    static get_blocks factory_from_data(reader& source);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size() const;

    static const std::string command;

    // 10 sequential hashes, then exponential samples until reaching genesis.
    block_locator start_hashes;
    hash_digest stop_hash;
};

BC_API bool operator==(const get_blocks& left, const get_blocks& right);
BC_API bool operator!=(const get_blocks& left, const get_blocks& right);

} // namspace message
} // namspace libbitcoin

#endif
