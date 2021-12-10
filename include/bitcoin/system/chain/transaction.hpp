/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API transaction
{
public:
    typedef std::vector<transaction> list;
    typedef std::shared_ptr<transaction> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default transaction is an invalid object.
    transaction();

    transaction(transaction&& other);
    transaction(const transaction& other);

    transaction(uint32_t version, uint32_t locktime, input::list&& inputs,
        output::list&& outputs);
    transaction(uint32_t version, uint32_t locktime, const input::list& inputs,
        const output::list& outputs);
    
    transaction(const data_slice& data, bool witness);
    transaction(std::istream& stream, bool witness);
    transaction(reader& source, bool witness);

    // Operators.
    // ------------------------------------------------------------------------

    transaction& operator=(transaction&& other);
    transaction& operator=(const transaction& other);

    bool operator==(const transaction& other) const;
    bool operator!=(const transaction& other) const;

    // Deserialization.
    // ------------------------------------------------------------------------

    bool from_data(const data_slice& data, bool witness);
    bool from_data(std::istream& stream, bool witness);
    bool from_data(reader& source, bool witness);

    /// Deserialization result.
    bool is_valid() const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool witness) const;
    void to_data(std::ostream& stream, bool witness) const;
    void to_data(writer& sink, bool witness) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    uint32_t version() const;
    uint32_t locktime() const;
    const input::list& inputs() const;
    const output::list& outputs() const;

    /// Computed properties.
    size_t weight() const;
    uint64_t fee() const;
    uint64_t claim() const;
    uint64_t value() const;
    hash_digest hash(bool witness) const;
    bool is_coinbase() const;
    bool is_segregated() const;
    size_t serialized_size(bool witness) const;

    // Methods.
    // ------------------------------------------------------------------------

    bool is_dusty(uint64_t minimum_output_value) const;
    size_t signature_operations(bool bip16, bool bip141) const;
    point::list points() const;
    hash_digest outputs_hash() const;
    hash_digest points_hash() const;
    hash_digest sequences_hash() const;

    // Guards (for tx pool without compact blocks).
    // ------------------------------------------------------------------------

    code guard() const;
    code guard(const context& state) const;

    // Validation (consensus checks).
    // ------------------------------------------------------------------------

    code check() const;
    code accept(const context& state) const;
    code connect(const context& state) const;

protected:
    ////friend class block;

    transaction(bool segregated, uint32_t version, uint32_t locktime,
        input::list&& inputs, output::list&& outputs, bool valid);
    transaction(bool segregated, uint32_t version, uint32_t locktime,
        const input::list& inputs, const output::list& outputs, bool valid);

    void reset();

    // Guard (context free).
    // ------------------------------------------------------------------------

    ////bool is_coinbase() const;
    bool is_internal_double_spend() const;
    bool is_oversized() const;

    // Guard (contextual).
    // ------------------------------------------------------------------------

    ////bool is_segregated() const;
    bool is_overweight() const;

    // prevouts required
    ////bool is_missing_prevouts() const;
    bool is_signature_operations_limit(bool bip16, bool bip141) const;

    // Check (context free).
    // ------------------------------------------------------------------------

    bool is_empty() const;
    bool is_null_non_coinbase() const;
    bool is_invalid_coinbase_size() const;

    // Accept (contextual).
    // ------------------------------------------------------------------------

    bool is_non_final(size_t height, uint32_t timestamp,
        uint32_t median_time_past, bool bip113) const;

    // prevouts required
    bool is_missing_prevouts() const;
    bool is_overspent() const;
    bool is_immature(size_t height) const;
    bool is_locked(size_t height, uint32_t median_time_past) const;

    // prevout confirmation state required
    bool is_unconfirmed_spend(size_t height) const;
    bool is_confirmed_double_spend(size_t height) const;

private:
    ////static size_t maximum_size(bool coinbase);
    static bool segregated(const input::list& inputs);

    // delegated
    code connect_input(const context& state, size_t index) const;

    bool segregated_;
    uint32_t version_;
    uint32_t locktime_;
    input::list inputs_;
    output::list outputs_;
    bool valid_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
