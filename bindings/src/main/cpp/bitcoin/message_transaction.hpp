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
#ifndef LIBBITCOIN__MESSAGE_TRANSACTION_HPP
#define LIBBITCOIN__MESSAGE_TRANSACTION_HPP

//#include <cstdint>
//#include <cstddef>
//#include <istream>
//#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/transaction.hpp>
//#include <bitcoin/bitcoin/chain/input.hpp>
//#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/chain_transaction.hpp>
//#include <bitcoin/bitcoin/message/version.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_transaction
//  : public chain::transaction
{
public:
//    typedef std::shared_ptr<transaction> ptr;
//    typedef std::shared_ptr<const transaction> const_ptr;
//    typedef std::vector<ptr> ptr_list;
//    typedef std::vector<const_ptr> const_ptr_list;
//    typedef std::shared_ptr<const_ptr_list> const_ptr_list_ptr;
//    typedef std::shared_ptr<const const_ptr_list> const_ptr_list_const_ptr;

    static message_transaction factory(uint32_t version, const utility_data_chunk& data);
//    static message_transaction factory(uint32_t version, std::istream& stream);
//    static message_transaction factory(uint32_t version, reader& source);

    message_transaction();

//    message_transaction(message_transaction&& other);
//    message_transaction(chain_transaction&& other);

    message_transaction(const message_transaction& other);
    message_transaction(const chain_transaction& other);

    message_transaction(uint32_t version, uint32_t locktime,
        chain_input_list&& inputs, chain_output_list&& outputs);
//    message_transaction(uint32_t version, uint32_t locktime,
//        const chain::input::list& inputs, const chain::output::list& outputs);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
//    size_t serialized_size(uint32_t version) const;

//    message_transaction& operator=(chain_transaction&& other);
    message_transaction& assign(chain_transaction&& other);

    /// This class is move assignable but not copy assignable.
//    message_transaction& operator=(message_transaction&& other);
    message_transaction& assign(message_transaction&& other);
//    void operator=(const message_transaction&) = delete;

//    bool operator==(const chain_transaction& other) const;
    bool eq(const chain_transaction& other) const;
//    bool operator!=(const chain_transaction& other) const;

//    bool operator==(const message_transaction& other) const;
    bool eq(const message_transaction& other) const;
//    bool operator!=(const message_transaction& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::transaction* getValue() {
        return value_;
    }

    void setValue(message::transaction* value) {
        value_ = value;
    }
private:
    message::transaction* value_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
