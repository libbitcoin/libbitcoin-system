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
#ifndef LIBBITCOIN__MESSAGE_BLOCK_TRANSACTIONS_HPP
#define LIBBITCOIN__MESSAGE_BLOCK_TRANSACTIONS_HPP

#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/block_transactions.hpp>
//#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <chain_transaction_list.hpp>
#include <math_hash_digest.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_block_transactions
{
public:
//    typedef std::shared_ptr<block_transactions> ptr;
//    typedef std::shared_ptr<const block_transactions> const_ptr;

    static message_block_transactions factory(uint32_t version, const utility_data_chunk& data);
//    static message_block_transactions factory(uint32_t version, std::istream& stream);
//    static message_block_transactions factory(uint32_t version, reader& source);

    message_block_transactions();
//    message_block_transactions(const math_hash_digest& block_hash,
//        const chain_transaction_list& transactions);
    message_block_transactions(math_hash_digest&& block_hash,
        chain_transaction_list&& transactions);
//    message_block_transactions(const message_block_transactions& other);
    message_block_transactions(message_block_transactions&& other);

    math_hash_digest& block_hash();
//    const math_hash_digest& block_hash() const;
//    void set_block_hash(const math_hash_digest& value);
    void set_block_hash(math_hash_digest&& value);

    chain_transaction_list& transactions();
//    const chain_transaction_list& transactions() const;
//    void set_transactions(const chain_transaction_list& other);
    void set_transactions(chain_transaction_list&& other);

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
//    message_block_transactions& operator=(message_block_transactions&& other);
    message_block_transactions& assign(message_block_transactions&& other);
//    void operator=(const message_block_transactions&) = delete;

//    bool operator==(const message_block_transactions& other) const;
    bool eq(const message_block_transactions& other) const;
//    bool operator!=(const message_block_transactions& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::block_transactions getValue() {
        return value;
    }

    void setValue(message::block_transactions value) {
        this->value = value;
    }
private:
    message::block_transactions value;
//    hash_digest block_hash_;
//    chain::transaction::list transactions_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
