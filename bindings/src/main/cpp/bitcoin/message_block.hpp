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
#ifndef LIBBITCOIN__MESSAGE_BLOCK_HPP
#define LIBBITCOIN__MESSAGE_BLOCK_HPP

//#include <cstdint>
//#include <cstddef>
//#include <istream>
//#include <memory>
#include <bitcoin/bitcoin/message/block.hpp>
#include <bitcoin/chain_block.hpp>
//#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/chain_transaction_list.hpp>
//#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_block
{
public:
//    typedef std::shared_ptr<block> ptr;
//    typedef std::shared_ptr<const block> const_ptr;
//    typedef std::vector<ptr> ptr_list;
//    typedef std::vector<const_ptr> const_ptr_list;
//    typedef std::shared_ptr<const_ptr_list> const_ptr_list_ptr;
//    typedef std::shared_ptr<const const_ptr_list> const_ptr_list_const_ptr;

    static message_block factory(uint32_t version, const utility_data_chunk& data);
//    static message_block factory(uint32_t version, std::istream& stream);
//    static message_block factory(uint32_t version, reader& source);

    message_block();

//    message_block(message_block&& other);
    message_block(const message_block& other);

//    message_block(chain_block&& other);
    message_block(const chain_block& other);

    message_block(chain_header&& header,
    		chain_transaction_list&& transactions);
//    message_block(const chain_header& header,
//        const chain_transaction_list& transactions);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    size_t serialized_size(uint32_t version) const;

//    message_block& operator=(chain::message_block&& other);
    message_block& assign(chain_block&& other);

    // This class is move assignable but not copy assignable.
//    message_block& operator=(message_block&& other);
    message_block& assign(message_block&& other);
//    void operator=(const message_block& other);

//    bool operator==(const chain::message_block& other) const;
    bool eq(const chain_block& other) const;
//    bool operator!=(const chain::message_block& other) const;

//    bool operator==(const message_block& other) const;
    bool eq(const message_block& other) const;
//    bool operator!=(const message_block& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::block* getValue() {
        return value_;
    }

    void setValue(message::block* value) {
        value_ = value;
    }
private:
    message::block* value_;

};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
