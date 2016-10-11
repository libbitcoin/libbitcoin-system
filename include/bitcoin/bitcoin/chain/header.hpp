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
#ifndef LIBBITCOIN_CHAIN_HEADER_HPP
#define LIBBITCOIN_CHAIN_HEADER_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API header
{
public:
    typedef std::vector<header> list;
    typedef std::shared_ptr<header> ptr;
    typedef std::shared_ptr<const header> const_ptr;
    typedef std::vector<header> ptr_list;
    typedef std::vector<const_ptr> const_ptr_list;

    // These properties facilitate block validation.
    // This validation data is not copied on block/header copy.
    struct validation
    {
        static const size_t orphan_height;

        size_t height = validation::orphan_height;
    };

    static header factory_from_data(const data_chunk& data,
        bool with_transaction_count=true);
    static header factory_from_data(std::istream& stream,
        bool with_transaction_count=true);
    static header factory_from_data(reader& source,
        bool with_transaction_count=true);
    static uint64_t satoshi_fixed_size_without_transaction_count();

    header();
    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, uint64_t transaction_count=0);
    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, uint64_t transaction_count=0);
    header(const header& other);
    header(header&& other);

    uint32_t version() const;
    void set_version(uint32_t value);

    hash_digest& previous_block_hash();
    const hash_digest& previous_block_hash() const;
    void set_previous_block_hash(const hash_digest& value);
    void set_previous_block_hash(hash_digest&& value);

    hash_digest& merkle();
    const hash_digest& merkle() const;
    void set_merkle(const hash_digest& value);
    void set_merkle(hash_digest&& value);

    uint32_t timestamp() const;
    void set_timestamp(uint32_t value);

    uint32_t bits() const;
    void set_bits(uint32_t value);

    uint32_t nonce() const;
    void set_nonce(uint32_t value);

    uint64_t transaction_count() const;
    void set_transaction_count(uint64_t value);

    bool from_data(const data_chunk& data, bool with_transaction_count=true);
    bool from_data(std::istream& stream, bool with_transaction_count=true);
    bool from_data(reader& source, bool with_transaction_count=true);
    data_chunk to_data(bool with_transaction_count=true) const;
    void to_data(std::ostream& stream, bool with_transaction_count=true) const;
    void to_data(writer& sink, bool with_transaction_count=true) const;

    void reset();
    bool is_valid() const;
    bool is_valid_time_stamp() const;
    bool is_valid_proof_of_work() const;

    code check() const;
    code accept(const chain_state& state) const;

    hash_digest hash() const;
    uint64_t serialized_size(bool with_transaction_count=true) const;

    /// This class is move assignable [but not copy assignable].
    header& operator=(header&& other);
    header& operator=(const header& other) /* = delete */;

    bool operator==(const header& other) const;
    bool operator!=(const header& other) const;

    // These fields do not participate in serialization or comparison.
    mutable validation validation;

private:
    mutable upgrade_mutex mutex_;
    mutable std::shared_ptr<hash_digest> hash_;

    uint32_t version_;
    hash_digest previous_block_hash_;
    hash_digest merkle_;
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;

    // The longest size (64) of a protocol variable int is deserialized here.
    // When writing a block the size of the transaction collection is used.
    uint64_t transaction_count_;
};

} // namespace chain
} // namespace libbitcoin

#endif
