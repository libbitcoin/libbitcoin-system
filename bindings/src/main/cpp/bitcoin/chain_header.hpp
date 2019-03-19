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
#ifndef LIBBITCOIN__CHAIN_HEADER_HPP
#define LIBBITCOIN__CHAIN_HEADER_HPP

//#include <cstddef>
//#include <cstdint>
//#include <istream>
//#include <string>
//#include <memory>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/chain/header.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>
#include <chain_chain_state.hpp>
#include <error_code.hpp>
#include <math_hash_digest.hpp>
#include <math_uint256_t.hpp>
#include <utility_data_chunk.hpp>

namespace libbitcoin {

//class settings;

//namespace chain {

namespace api {

class BC_API chain_header
{
public:
//    typedef std::vector<header> list;
//    typedef std::shared_ptr<header> ptr;
//    typedef std::shared_ptr<const header> const_ptr;
//    typedef std::vector<ptr> ptr_list;
//    typedef std::vector<const_ptr> const_ptr_list;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
//    struct validation
//    {
//        uint64_t originator = 0;
//        uint32_t median_time_past = 0;
//        chain_state::ptr state = nullptr;
//
//        /// The block validation error code (if validated).
//        code error = error::success;
//
//        /// Header exists, in any state (don't download it).
//        bool exists = false;
//
//        /// Block transactions are populated (don't download block/txs).
//        bool populated = false;
//
//        /// Block has been validated (don't re-validate).
//        bool validated = false;
//
//        /// Header is a candidate (and is not yet invalid).
//        bool candidate = false;
//
//        /// Block is confirmed, relative to queried fork point (and is valid).
//        bool confirmed = false;
//    };

    // Constructors.
    //-------------------------------------------------------------------------

    chain_header();

//    header(header&& other);
    chain_header(const chain_header& other);

    chain_header(uint32_t version, const math_hash_digest& previous_block_hash,
        const math_hash_digest& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce);
//    header(uint32_t version, math_hash_digest&& previous_block_hash,
//        math_hash_digest&& merkle, uint32_t timestamp, uint32_t bits,
//        uint32_t nonce);

    // Operators.
    //-------------------------------------------------------------------------

//    header& operator=(header&& other);
    chain_header& assign(const chain_header& other);
//    header& operator=(const header& other);

    bool eq(const chain_header& other) const;
//    bool operator==(const header& other) const;
//    bool operator!=(const header& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static chain_header factory(const utility_data_chunk& data, bool wire=true);
//    static header factory(std::istream& stream, bool wire=true);
//    static header factory(reader& source, bool wire=true);
//    static header factory(reader& source, math_hash_digest&& hash, bool wire=true);
//    static header factory(reader& source, const math_hash_digest& hash,
//        bool wire=true);

    bool from_data(const utility_data_chunk& data, bool wire=true);
//    bool from_data(std::istream& stream, bool wire=true);
//    bool from_data(reader& source, bool wire=true);
//    bool from_data(reader& source, math_hash_digest&& hash, bool wire=true);
//    bool from_data(reader& source, const math_hash_digest& hash, bool wire=true);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool wire=true) const;
//    void to_data(std::ostream& stream, bool wire=true) const;
//    void to_data(writer& sink, bool wire=true) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    static size_t satoshi_fixed_size();
    size_t serialized_size(bool wire=true) const;

    uint32_t version() const;
    void set_version(uint32_t value);

    const math_hash_digest& previous_block_hash() const;
    void set_previous_block_hash(const math_hash_digest& value);
//    void set_previous_block_hash(math_hash_digest&& value);

    /// This may not match the computed value, validation compares them.
    const math_hash_digest& merkle() const;
    void set_merkle(const math_hash_digest& value);
//    void set_merkle(math_hash_digest&& value);

    uint32_t timestamp() const;
    void set_timestamp(uint32_t value);

    uint32_t bits() const;
    void set_bits(uint32_t value);

    uint32_t nonce() const;
    void set_nonce(uint32_t value);

    math_hash_digest hash() const;

    // Validation.
    //-------------------------------------------------------------------------

    math_uint256_t proof() const;
    static math_uint256_t proof(uint32_t bits);

    bool is_valid_timestamp(uint32_t timestamp_limit_seconds) const;
    bool is_valid_proof_of_work(uint32_t proof_of_work_limit,
        bool scrypt=false) const;

    error_code check(uint32_t timestamp_limit_seconds, uint32_t proof_of_work_limit,
        bool scrypt=false) const;
    error_code accept() const;
    error_code accept(const chain_chain_state& state) const;

    chain::header getValue() {
        return value;
    }

    void setValue(chain::header value) {
        this->value = value;
    }
    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
//    mutable validation metadata;

//protected:
//    // So that block may call reset from its own.
//    friend class block;
//
//    void reset();
//    void invalidate_cache() const;

private:
    chain::header value;
//    typedef std::shared_ptr<math_hash_digest> hash_ptr;
//
//    hash_ptr hash_cache() const;
//
//    mutable hash_ptr hash_;
//    mutable upgrade_mutex mutex_;
//
//    uint32_t version_;
//    math_hash_digest previous_block_hash_;
//    math_hash_digest merkle_;
//    uint32_t timestamp_;
//    uint32_t bits_;
//    uint32_t nonce_;
};

} // api
//} // namespace chain
} // namespace libbitcoin

#endif
