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
#ifndef LIBBITCOIN_CHAIN_BLOCK_HPP
#define LIBBITCOIN_CHAIN_BLOCK_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API block
{
public:
    typedef std::vector<block> list;
    typedef std::shared_ptr<block> ptr;
    typedef std::vector<ptr> ptr_list;
    typedef std::vector<size_t> indexes;

    static block factory_from_data(const data_chunk& data,
        bool with_transaction_count = true);
    static block factory_from_data(std::istream& stream,
        bool with_transaction_count = true);
    static block factory_from_data(reader& source,
        bool with_transaction_count = true);
    static hash_digest generate_merkle_root(
        const transaction::list& transactions);
    static block genesis_mainnet();
    static block genesis_testnet();

    block();
    block(const block& other);
    block(const chain::header& header,
        const chain::transaction::list& transactions);

    block(block&& other);
    block(chain::header&& header,
        chain::transaction::list&& transactions);

    /// This class is move assignable but not copy assignable.
    block& operator=(block&& other);
    void operator=(const block&) = delete;

    bool from_data(const data_chunk& data, bool with_transaction_count = true);
    bool from_data(std::istream& stream, bool with_transaction_count = true);
    bool from_data(reader& source, bool with_transaction_count = true);
    data_chunk to_data(bool with_transaction_count = true) const;
    void to_data(std::ostream& stream, bool with_transaction_count = true) const;
    void to_data(writer& sink, bool with_transaction_count = true) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(bool with_transaction_count = true) const;

    chain::header header;
    transaction::list transactions;
};

} // namespace chain
} // namespace libbitcoin

#endif
