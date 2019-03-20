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
#ifndef LIBBITCOIN__MESSAGE_MERKLE_BLOCK_HPP
#define LIBBITCOIN__MESSAGE_MERKLE_BLOCK_HPP

//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/merkle_block.hpp>
#include <chain_block.hpp>
#include <chain_header.hpp>
#include <math_hash_list.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_merkle_block
{
public:
//    typedef std::vector<merkle_block> list;
//    typedef std::shared_ptr<merkle_block> ptr;
//    typedef std::shared_ptr<const merkle_block> const_ptr;

    static message_merkle_block factory(uint32_t version, const utility_data_chunk& data);
//    static message_merkle_block factory(uint32_t version, std::istream& stream);
//    static message_merkle_block factory(uint32_t version, reader& source);

    message_merkle_block();
//    message_merkle_block(const chain::header& header, size_t total_transactions,
//        const math_hash_list& hashes, const utility_data_chunk& flags);
    message_merkle_block(chain_header&& header, size_t total_transactions,
        math_hash_list&& hashes, utility_data_chunk&& flags);
    message_merkle_block(const chain_block& block);
//    message_merkle_block(const message_merkle_block& other);
    message_merkle_block(message_merkle_block&& other);

    chain_header& header();
//    const chain::header& header() const;
//    void set_header(const chain::header& value);
    void set_header(chain_header&& value);

    size_t total_transactions() const;
    void set_total_transactions(size_t value);

    math_hash_list& hashes();
//    const math_hash_list& hashes() const;
//    void set_hashes(const math_hash_list& value);
    void set_hashes(math_hash_list&& value);

    utility_data_chunk& flags();
//    const utility_data_chunk& flags() const;
//    void set_flags(const utility_data_chunk& value);
    void set_flags(utility_data_chunk&& value);

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
//    message_merkle_block& operator=(message_merkle_block&& other);
    message_merkle_block& assign(message_merkle_block&& other);
//    void operator=(const message_merkle_block&) = delete;

//    bool operator==(const message_merkle_block& other) const;
    bool eq(const message_merkle_block& other) const;
//    bool operator!=(const message_merkle_block& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;

public:
    message::merkle_block getValue() {
        return value;
    }

    void setValue(message::merkle_block value) {
        this->value = value;
    }
private:
    message::merkle_block value;
//    chain::header header_;
//    size_t total_transactions_;
//    math_hash_list hashes_;
//    utility_data_chunk flags_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
