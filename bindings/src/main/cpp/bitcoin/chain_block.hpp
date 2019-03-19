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
#ifndef LIBBITCOIN__CHAIN_BLOCK_HPP
#define LIBBITCOIN__CHAIN_BLOCK_HPP

//#include <cstddef>
//#include <cstdint>
//#include <istream>
//#include <memory>
//#include <string>
//#include <vector>
//#include <boost/optional.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <chain_block_indexes.hpp>
//#include <chain_chain_state.hpp>
#include <chain_header.hpp>
//#include <chain_transaction.hpp>
#include <chain_transaction_list.hpp>
//#include <error_code.hpp>
//#include <bitcoin/bitcoin/config/settings.hpp>
//#include <math_hash_digest.hpp>
#include <math_hash_list.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
//#include <bitcoin/bitcoin/utility/asio.hpp>
//#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {

//class settings;

//namespace chain {
namespace api {

class BC_API chain_block
{
public:

    // Constructors.
    //-------------------------------------------------------------------------

    chain_block();

    //chain_block(chain_block&& other);
    chain_block(const chain_block& other);

    //chain_block(chain_header&& header, chain_transaction_list&& transactions);
    chain_block(const chain_header& header, const chain_transaction_list& transactions);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable but NOT copy assignable (performance).
    void assign(chain_block&& other);
    //block& operator=(block&& other);
    //block& operator=(const block& other) = delete;

    bool eq(const chain_block& other);
    //bool operator==(const block& other) const;
    //bool operator!=(const block& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static chain_block factory(const utility_data_chunk& data, bool witness=false);
//    static block factory(std::istream& stream, bool witness=false);
//    static block factory(reader& source, bool witness=false);

    bool from_data(const utility_data_chunk& data, bool witness=false);
//    bool from_data(std::istream& stream, bool witness=false);
//    bool from_data(reader& source, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool witness=false) const;
//    void to_data(std::ostream& stream, bool witness=false) const;
//    void to_data(writer& sink, bool witness=false) const;
    math_hash_list to_hashes(bool witness=false) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool witness=false) const;

    const chain_header& header() const;
    void set_header(const chain_header& value);
//    void set_header(chain::header&& value);

    const chain_transaction_list& transactions() const;
    void set_transactions(const chain_transaction_list& value);
//    void set_transactions(transaction::list&& value);

    math_hash_digest hash() const;

    // Utilities.
    //-------------------------------------------------------------------------

    static size_t locator_size(size_t top);
    static chain_block_indexes locator_heights(size_t top);

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    static uint64_t subsidy(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi);

    uint64_t fees() const;
    uint64_t claim() const;
    uint64_t reward(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi) const;
    math_hash_digest generate_merkle_root(bool witness=false) const;
    size_t signature_operations() const;
    size_t signature_operations(bool bip16, bool bip141) const;
    size_t total_non_coinbase_inputs() const;
    size_t total_inputs() const;
    size_t weight() const;

    bool is_extra_coinbases() const;
    bool is_final(size_t height, uint32_t block_time) const;
    bool is_distinct_transaction_set() const;
    bool is_valid_coinbase_claim(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi) const;
    bool is_valid_coinbase_script(size_t height) const;
    bool is_valid_witness_commitment() const;
    bool is_forward_reference() const;
    bool is_internal_double_spend() const;
    bool is_valid_merkle_root() const;
    bool is_segregated() const;

    error_code check(uint64_t max_money, uint32_t timestamp_limit_seconds,
        uint32_t proof_of_work_limit, bool scrypt=false) const;
    error_code check_transactions(uint64_t max_money) const;
    error_code accept(const libbitcoin::config::settings& settings, bool transactions=true,
        bool header=true) const;
    error_code accept(const chain_chain_state& state, const libbitcoin::config::settings& settings,
        bool transactions=true, bool header=true) const;
    error_code accept_transactions(const chain_chain_state& state) const;
    error_code connect() const;
    error_code connect(const chain_chain_state& state) const;
    error_code connect_transactions(const chain_chain_state& state) const;

    chain::block getValue() {
        return value;
    }

    void setValue(chain::block value) {
        this->value = value;
    }
    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
//    mutable block::validation metadata;

//protected:
//    void reset();
private:
    chain::block value;

};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif
