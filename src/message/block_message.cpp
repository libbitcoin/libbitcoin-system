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
#include <bitcoin/bitcoin/message/block_message.hpp>

#include <cstdint>
#include <cstddef>
#include <istream>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
namespace message {

block_message block_message::factory_from_data(const data_chunk& data,
    bool with_transaction_count)
{
    block_message instance;
    instance.from_data(data, with_transaction_count);
    return instance;
}

block_message block_message::factory_from_data(std::istream& stream,
    bool with_transaction_count)
{
    block_message instance;
    instance.from_data(stream, with_transaction_count);
    return instance;
}

block_message block_message::factory_from_data(reader& source,
    bool with_transaction_count)
{
    block_message instance;
    instance.from_data(source, with_transaction_count);
    return instance;
}

uint64_t block_message::originator() const
{
    return originator_;
}

void block_message::set_originator(uint64_t value)
{
    originator_ = value;
}

} // namspace message
} // namspace libbitcoin
