/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_HPP
#define LIBBITCOIN__MESSAGE_INVENTORY_VECTOR_HPP

//#include <cstdint>
//#include <istream>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <math_hash_digest.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_inventory_vector : public message::inventory_vector
{
public:
//    typedef std::vector<inventory_vector> list;

//    enum class type_id : uint32_t
//    {
//        error = 0,
//        transaction = 1,
//        block = 2,
//        filtered_block = 3,
//        compact_block = 4,
//        witness = (1u << 30),
//        witness_transaction = witness | transaction,
//        witness_block = witness | block,
//        reserved = witness | filtered_block
//    };

    static type_id to_type(uint32_t value);
    static uint32_t to_number(type_id type);
    static std::string to_string(type_id type);

    static message_inventory_vector factory(uint32_t version, const utility_data_chunk& data);
//    static message_inventory_vector factory(uint32_t version, std::istream& stream);
//    static message_inventory_vector factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_inventory_vector();
//    message_inventory_vector(type_id type, const math_hash_digest& hash);
    message_inventory_vector(type_id type, math_hash_digest&& hash);
//    message_inventory_vector(const message_inventory_vector& other);
    message_inventory_vector(message_inventory_vector&& other);

    type_id type() const;
    void set_type(type_id value);

    math_hash_digest& hash();
//    const math_hash_digest& hash() const;
//    void set_hash(const math_hash_digest& value);
    void set_hash(math_hash_digest&& value);

    bool is_block_type() const;
    bool is_transaction_type() const;

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    void to_witness();
    size_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
//    message_inventory_vector& operator=(message_inventory_vector&& other);
    message_inventory_vector& assign(message_inventory_vector&& other);
//    void operator=(const message_inventory_vector& other);

//    bool operator==(const message_inventory_vector& other) const;
    bool eq(const message_inventory_vector& other) const;
//    bool operator!=(const message_inventory_vector& other) const;

//private:
//    type_id type_;
//    math_hash_digest hash_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
