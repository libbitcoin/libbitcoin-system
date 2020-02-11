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
#ifndef LIBBITCOIN__MESSAGE_COMPACT_BLOCK_HPP
#define LIBBITCOIN__MESSAGE_COMPACT_BLOCK_HPP

//#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/compact_block.hpp>
#include <bitcoin/chain_header.hpp>
#include <bitcoin/math_mini_hash_list.hpp>
#include <bitcoin/message_prefilled_transaction_list.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_compact_block
{
public:
//    typedef std::shared_ptr<compact_block> ptr;
//    typedef std::shared_ptr<const compact_block> const_ptr;
//
//    typedef mini_hash short_id;
//    typedef mini_hash_list math_mini_hash_list;

    static message_compact_block factory(uint32_t version, const utility_data_chunk& data);
//    static message_compact_block factory(uint32_t version, std::istream& stream);
//    static message_compact_block factory(uint32_t version, reader& source);

    message_compact_block();
//    message_compact_block(const chain_header& header, uint64_t nonce,
//        const math_mini_hash_list& short_ids,
//        const message_prefilled_transaction_list& transactions);
    message_compact_block(chain_header&& header, uint64_t nonce,
        math_mini_hash_list&& short_ids,
        message_prefilled_transaction_list&& transactions);
//    message_compact_block(const message_compact_block& other);
//    message_compact_block(message_compact_block&& other);

    chain_header& header();
//    const chain_header& header() const;
//    void set_header(const chain_header& value);
    void set_header(chain_header&& value);

    uint64_t nonce() const;
    void set_nonce(uint64_t value);

    math_mini_hash_list& short_ids();
//    const math_mini_hash_list& short_ids() const;
//    void set_short_ids(const math_mini_hash_list& value);
    void set_short_ids(math_mini_hash_list&& value);

    message_prefilled_transaction_list& transactions();
//    const message_prefilled_transaction_list& transactions() const;
//    void set_transactions(const message_prefilled_transaction_list& value);
    void set_transactions(message_prefilled_transaction_list&& value);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
//    message_compact_block& operator=(message_compact_block&& other);
    message_compact_block& assign(message_compact_block&& other);
//    void operator=(const message_compact_block&) = delete;

//    bool operator==(const message_compact_block& other) const;
    bool eq(const message_compact_block& other) const;
//    bool operator!=(const message_compact_block& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::compact_block* getValue() {
        return value_;
    }

    void setValue(message::compact_block* value) {
        value_ = value;
    }
private:
    message::compact_block* value_;
//    chain_header header_;
//    uint64_t nonce_;
//    math_mini_hash_list short_ids_;
//    message_prefilled_transaction_list transactions_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
