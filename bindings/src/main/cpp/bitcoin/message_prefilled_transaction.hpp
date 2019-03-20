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
#ifndef LIBBITCOIN__MESSAGE_PREFILLED_TRANSACTION_HPP
#define LIBBITCOIN__MESSAGE_PREFILLED_TRANSACTION_HPP

//#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/prefilled_transaction.hpp>
//#include <bitcoin/bitcoin/chain/transaction.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_prefilled_transaction
{
public:
//    typedef std::vector<prefilled_transaction> list;
//    typedef std::shared_ptr<const prefilled_transaction> const_ptr;

    static message_prefilled_transaction factory(uint32_t version, const utility_data_chunk& data);
//    static message_prefilled_transaction factory(uint32_t version, std::istream& stream);
//    static message_prefilled_transaction factory(uint32_t version, reader& source);

    message_prefilled_transaction();
//    message_prefilled_transaction(uint64_t index, const chain::transaction& tx);
    message_prefilled_transaction(uint64_t index, chain::transaction&& tx);
//    message_prefilled_transaction(const message_prefilled_transaction& other);
    message_prefilled_transaction(message_prefilled_transaction&& other);

    uint64_t index() const;
    void set_index(uint64_t value);

    chain::transaction& transaction();
//    const chain::transaction& transaction() const;
//    void set_transaction(const chain::transaction& tx);
    void set_transaction(chain::transaction&& tx);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

//    message_prefilled_transaction& operator=(message_prefilled_transaction&& other);
    message_prefilled_transaction& assign(message_prefilled_transaction&& other);
//    message_prefilled_transaction& operator=(const message_prefilled_transaction& other);

//    bool operator==(const message_prefilled_transaction& other) const;
    bool eq(const message_prefilled_transaction& other) const;
//    bool operator!=(const message_prefilled_transaction& other) const;

public:
    message::prefilled_transaction getValue() {
        return value;
    }

    void setValue(message::prefilled_transaction value) {
        this->value = value;
    }
private:
    message::prefilled_transaction value;
//    uint64_t index_;
//    chain::transaction transaction_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
