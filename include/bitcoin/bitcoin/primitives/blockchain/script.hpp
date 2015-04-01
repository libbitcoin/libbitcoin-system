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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_SCRIPT_HPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_SCRIPT_HPP

//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/operation.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/payment.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

class BC_API script_type
{
public:

    operation_stack operations;

    script_type();

    script_type(const data_chunk& value, bool allow_raw_data_fallback = false);

    template<typename Iterator, bool SafeCheckLast>
    script_type(deserializer<Iterator, SafeCheckLast>& deserial,
        bool allow_raw_data_fallback = false);

    template<typename Iterator>
    script_type(const Iterator begin, const Iterator end,
        bool allow_raw_data_fallback = false);

    script_type(const operation& op);

    script_type(const std::string& human_readable);

    operator const data_chunk() const;

    size_t satoshi_size() const;

    std::string to_string() const;

    payment_type type() const;

private:

    template<typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial,
        bool allow_raw_data_fallback);

    void deserialize(data_chunk raw_script, bool allow_raw_data_fallback);

    void parse(data_chunk raw_script);

    template<typename Deserializer>
    size_t read_operation_data_byte_count(opcode code, uint8_t raw_byte,
        Deserializer& deserial);
};

}

#include <bitcoin/bitcoin/impl/primitives/blockchain/script.ipp>

#endif
