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
#ifndef LIBBITCOIN_CHAIN_SCRIPT_IPP
#define LIBBITCOIN_CHAIN_SCRIPT_IPP

#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace chain {

template <typename Iterator, bool SafeCheckLast>
void script::deserialize(deserializer<Iterator, SafeCheckLast>& deserial,
    bool allow_raw_data_fallback)
{
    auto script_length = deserial.read_variable_uint();
    BITCOIN_ASSERT(script_length <= max_uint32);

    auto script_length32 = static_cast<uint32_t>(script_length);
    data_chunk raw_script = deserial.read_data(script_length32);

    deserialize(raw_script, allow_raw_data_fallback);
}

template <typename Iterator, bool SafeCheckLast>
script::script(deserializer<Iterator, SafeCheckLast>& deserial,
    bool allow_raw_data_fallback)
{
    deserialize(deserial, allow_raw_data_fallback);
}

template<typename Iterator>
script::script(const Iterator begin, const Iterator end,
    bool allow_raw_data_fallback)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial, allow_raw_data_fallback);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

template <typename Deserializer>
size_t script::read_operation_data_byte_count(opcode code,
    uint8_t raw_byte, Deserializer& deserial)
{
    switch (code)
    {
        case opcode::special:
            return raw_byte;
        case opcode::pushdata1:
            return deserial.read_byte();
        case opcode::pushdata2:
            return deserial.read_2_bytes();
        case opcode::pushdata4:
            return deserial.read_4_bytes();
        default:
            break;
    }
    BITCOIN_ASSERT_MSG(false, "Invalid opcode passed to function.");

    return 0;
}

} // end chain
} // end libbitcoin

#endif
