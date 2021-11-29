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
    //-------------------------------------------------------------------------

    transaction();

    transaction(transaction&& other);
    transaction(const transaction& other);

    transaction(uint32_t version, uint32_t locktime, input::list&& inputs,
        output::list&& outputs);
    transaction(uint32_t version, uint32_t locktime, const input::list& inputs,
        const output::list& outputs);
    
    transaction(const data_chunk& data, bool witness=false);
    transaction(std::istream& stream, bool witness=false);
    transaction(reader& source, bool witness=false);

    // Operators.
    //-------------------------------------------------------------------------

    transaction& operator=(transaction&& other);
    transaction& operator=(const transaction& other);

    bool operator==(const transaction& other) const;
    bool operator!=(const transaction& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    bool from_data(const data_chunk& data, bool witness=false);
    bool from_data(std::istream& stream, bool witness=false);
    bool from_data(reader& source, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool witness=false) const;
    void to_data(std::ostream& stream, bool witness=false) const;
    void to_data(writer& sink, bool witness=false) const;

    // Properties.
    //-------------------------------------------------------------------------

    static size_t maximum_size(bool is_coinbase);
    size_t serialized_size(bool witness=false) const;

    uint32_t version() const;
    uint32_t locktime() const;

    const input::list& inputs() const;
    const output::list& outputs() const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    hash_digest outputs_hash() const;
    hash_digest points_hash() const;
    hash_digest sequences_hash() const;
    hash_digest hash(bool witness = false) const;

    uint64_t total_output_value() const;
    point::list points() const;
    size_t weight() const;

    bool is_coinbase() const;
    bool is_null_non_coinbase() const;
    bool is_oversized_coinbase() const;
    bool is_internal_double_spend() const;
    bool is_dusty(uint64_t minimum_output_value) const;
    bool is_final(size_t block_height, uint32_t block_time) const;
    bool is_locktime_conflict() const;
    bool is_segregated() const;

    code check(uint64_t max_money, bool pool=true) const;
    code accept(const context& state, bool pool=true) const;
    code connect(const context& state) const;
    code connect_input(const context& state, size_t input_index) const;

protected:
    static bool is_segregated(const input::list& inputs);

    transaction(bool segregated, uint32_t version, uint32_t locktime,
        input::list&& inputs, output::list&& outputs, bool valid);
    transaction(bool segregated, uint32_t version, uint32_t locktime,
        const input::list& inputs, const output::list& outputs, bool valid);

    void reset();

private:
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
