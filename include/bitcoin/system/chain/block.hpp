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
#ifndef LIBBITCOIN_CHAIN_BLOCK_HPP
#define LIBBITCOIN_CHAIN_BLOCK_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API block
{
public:
    typedef std::vector<block> list;
    typedef std::vector<size_t> indexes;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation
    {
        uint64_t originator = 0;
        code error = error::not_found;
        chain_state::ptr state = nullptr;

        // Similate organization and instead just validate the block.
        bool simulate = false;

        asio::time_point start_deserialize;
        asio::time_point end_deserialize;
        asio::time_point start_check;
        asio::time_point start_populate;
        asio::time_point start_accept;
        asio::time_point start_connect;
        asio::time_point start_notify;
        asio::time_point start_pop;
        asio::time_point start_push;
        asio::time_point end_push;
        float cache_efficiency;
    };

    // Constructors.
    //-------------------------------------------------------------------------

    block();

    block(block&& other);
    block(const block& other);

    block(chain::header&& header, transaction::list&& transactions);
    block(const chain::header& header, const transaction::list& transactions);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable but NOT copy assignable.
    block& operator=(block&& other);
    block& operator=(const block& other) = delete;

    bool operator==(const block& other) const;
    bool operator!=(const block& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static block factory_from_data(const data_chunk& data, bool witness=false);
    static block factory_from_data(std::istream& stream, bool witness=false);
    static block factory_from_data(reader& source, bool witness=false);

    bool from_data(const data_chunk& data, bool witness=false);
    bool from_data(std::istream& stream, bool witness=false);
    bool from_data(reader& source, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool witness=false) const;
    void to_data(std::ostream& stream, bool witness=false) const;
    void to_data(writer& sink, bool witness=false) const;
    hash_list to_hashes(bool witness=false) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool witness=false) const;

    // deprecated (unsafe)
    chain::header& header();

    const chain::header& header() const;
    void set_header(const chain::header& value);
    void set_header(chain::header&& value);

    // deprecated (unsafe)
    transaction::list& transactions();

    const transaction::list& transactions() const;
    void set_transactions(const transaction::list& value);
    void set_transactions(transaction::list&& value);

    hash_digest hash() const;

    // Utilities.
    //-------------------------------------------------------------------------

    static block genesis_mainnet();
    static block genesis_testnet();
    static block genesis_regtest();
    static size_t locator_size(size_t top);
    static indexes locator_heights(size_t top);

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    static uint64_t subsidy(size_t height, bool retarget=true);
    static uint256_t proof(uint32_t bits);

    uint64_t fees() const;
    uint64_t claim() const;
    uint64_t reward(size_t height) const;
    uint256_t proof() const;
    hash_digest generate_merkle_root(bool witness=false) const;
    size_t signature_operations() const;
    size_t signature_operations(bool bip16, bool bip141) const;
    size_t total_inputs(bool with_coinbase=true) const;
    size_t weight() const;

    bool is_extra_coinbases() const;
    bool is_final(size_t height, uint32_t block_time) const;
    bool is_distinct_transaction_set() const;
    bool is_valid_coinbase_claim(size_t height) const;
    bool is_valid_coinbase_script(size_t height) const;
    bool is_valid_witness_commitment() const;
    bool is_forward_reference() const;
    bool is_internal_double_spend() const;
    bool is_valid_merkle_root() const;
    bool is_segregated() const;

    code check() const;
    code check_transactions() const;
    code accept(bool transactions=true) const;
    code accept(const chain_state& state, bool transactions=true) const;
    code accept_transactions(const chain_state& state) const;
    code connect() const;
    code connect(const chain_state& state) const;
    code connect_transactions(const chain_state& state) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation validation;

protected:
    void reset();
    size_t non_coinbase_input_count() const;

private:
    chain::header header_;
    transaction::list transactions_;

    // These share a mutext as they are not expected to contend.
    mutable boost::optional<bool> segregated_;
    mutable boost::optional<size_t> total_inputs_;
    mutable boost::optional<size_t> base_size_;
    mutable boost::optional<size_t> total_size_;
    mutable upgrade_mutex mutex_;
};

} // namespace chain
} // namespace libbitcoin

#endif
