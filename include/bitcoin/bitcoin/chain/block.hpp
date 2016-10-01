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

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API block
{
public:
    typedef std::vector<size_t> indexes;

    // These properties facilitate block validation.
    // This validation data is not copied on block copy.
    struct validation
    {
        static const size_t orphan_height;

        size_t height = validation::orphan_height;
        code result = error::not_found;
        chain_state::ptr state = nullptr;
    };

    static block factory_from_data(const data_chunk& data);
    static block factory_from_data(std::istream& stream);
    static block factory_from_data(reader& source);

    static hash_number difficulty(uint32_t bits);
    static uint64_t subsidy(size_t height);
    static size_t locator_size(size_t top);
    static indexes locator_heights(size_t top);
    static block genesis_mainnet();
    static block genesis_testnet();

    block();
    block(const chain::header& header, const transaction::list& transactions);
    block(chain::header&& header, transaction::list&& transactions);
    block(const block& other);
    block(block&& other);

    chain::header& header();
    const chain::header& header() const;
    void set_header(const chain::header& value);
    void set_header(chain::header&& value);

    transaction::list& transactions();
    const transaction::list& transactions() const;
    void set_transactions(const transaction::list& value);
    void set_transactions(transaction::list&& value);

    bool is_extra_coinbases() const;
    bool is_final(size_t height) const;
    bool is_valid_merkle_root() const;
    bool is_distinct_transaction_set() const;
    bool is_valid_coinbase_claim(size_t height) const;
    bool is_valid_coinbase_script(size_t height) const;

    hash_digest hash() const;

    code check() const;
    code check_transactions() const;
    code accept() const;
    code accept(const chain_state& state) const;
    code accept_transactions(const chain_state& state) const;
    code connect() const;
    code connect(const chain_state& state) const;
    code connect_transactions(const chain_state& state) const;

    uint64_t fees() const;
    uint64_t claim() const;
    uint64_t reward(size_t height) const;

    hash_number difficulty() const;
    hash_digest generate_merkle_root() const;
    size_t total_inputs(bool with_coinbase_transaction=true) const;
    size_t signature_operations(bool bip16_active) const;
    size_t signature_operations() const;

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size() const;

    /// This class is move assignable [but not copy assignable].
    block& operator=(block&& other);
    block& operator=(const block& other) /* = delete */;

    bool operator==(const block& other) const;
    bool operator!=(const block& other) const;

    // These fields do not participate in serialization or comparison.
    //-------------------------------------------------------------------------
    mutable validation validation;

private:
    static hash_digest build_merkle_tree(hash_list& merkle);

    chain::header header_;
    transaction::list transactions_;
};

} // namespace chain
} // namespace libbitcoin

#endif
