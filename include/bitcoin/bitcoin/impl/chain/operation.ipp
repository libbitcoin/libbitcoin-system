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
#ifndef LIBBITCOIN_CHAIN_OPERATION_IPP
#define LIBBITCOIN_CHAIN_OPERATION_IPP

namespace libbitcoin {
namespace chain {

template <typename Iterator, bool SafeCheckLast>
void operation::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    uint8_t raw_byte = deserial.read_byte();

    code_ = static_cast<opcode>(raw_byte);

    BITCOIN_ASSERT(raw_byte != 0 || code_ == opcode::zero);

    if (0 < raw_byte && raw_byte <= 75)
        code_ = opcode::special;

    if (operation::must_read_data(code_))
    {
        size_t read_n_bytes =
            read_opcode_data_byte_count(code_, raw_byte, deserial);

        data_ = deserial.read_data(read_n_bytes);

        BITCOIN_ASSERT(read_n_bytes || data_.empty());
    }
}

template <typename Iterator, bool SafeCheckLast>
operation::operation(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
operation::operation(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

template <typename Deserializer>
size_t operation::read_opcode_data_byte_count(opcode code,
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
