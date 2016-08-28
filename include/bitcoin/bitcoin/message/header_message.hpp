/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_HEADER_MESSAGE_HPP
#define LIBBITCOIN_MESSAGE_HEADER_MESSAGE_HPP

#include <cstdint>
#include <cstddef>
#include <istream>
#include <memory>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
namespace message {

class BC_API header_message
  : public chain::header
{
public:
    typedef std::vector<header_message> list;
    typedef std::shared_ptr<header_message> ptr;
    typedef std::vector<ptr> ptr_list;

    static header_message factory_from_data(const uint32_t version,
        const data_chunk& data, bool with_transaction_count=true);
    static header_message factory_from_data(const uint32_t version,
        std::istream& stream, bool with_transaction_count=true);
    static header_message factory_from_data(const uint32_t version,
        reader& source, bool with_transaction_count=true);

    header_message();
    header_message(const chain::header& other);
    header_message(const header_message& other);
    header_message(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, uint64_t transaction_count=0);

    header_message(chain::header&& other);
    header_message(header_message&& other);
    header_message(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, uint64_t transaction_count = 0);

    /// This class is move assignable but not copy assignable.
    header_message& operator=(header_message&& other);
    void operator=(const header_message&) = delete;

    bool from_data(const uint32_t version, const data_chunk& data,
        bool with_transaction_count = true);
    bool from_data(const uint32_t version, std::istream& stream,
        bool with_transaction_count = true);
    bool from_data(const uint32_t version, reader& source,
        bool with_transaction_count = true);
    data_chunk to_data(const uint32_t version,
        bool with_transaction_count = true) const;
    void to_data(const uint32_t version, std::ostream& stream,
        bool with_transaction_count = true) const;
    void to_data(const uint32_t version, writer& sink,
        bool with_transaction_count = true) const;
    uint64_t serialized_size(const uint32_t version,
        bool with_transaction_count = true) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

private:
    uint64_t originator_;
};

} // namespace message
} // namespace libbitcoin

#endif
