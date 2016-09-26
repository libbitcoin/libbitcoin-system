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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/input.hpp>
#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction
{
public:
    typedef std::vector<transaction> list;

    // This metadata is not copied on tx copy.
    // These properties facilitate transaction pool processing.
    struct metadata
    {
        typedef std::function<void(const code&)> confirm_handler;

        confirm_handler confirm = nullptr;
    };

    // This validation data is not copied on tx copy.
    // These properties facilitate block and transaction validation.
    struct validation
    {
        /// This does not exclude the two excepted transactions (see BIP30).
        /// The transaction hash duplicates one in the blockchain. For block
        /// validation this may be populated only from the blockchain and
        /// otherwise may (or may not) consider the transaction pool.
        bool duplicate = false;
    };

    static transaction factory_from_data(const data_chunk& data,
        bool satoshi=true);
    static transaction factory_from_data(std::istream& stream,
        bool satoshi=true);
    static transaction factory_from_data(reader& source, bool satoshi=true);

    transaction();
    transaction(uint32_t version, uint32_t locktime, const input::list& inputs,
        const output::list& outputs);
    transaction(uint32_t version, uint32_t locktime, input::list&& inputs,
        output::list&& outputs);
    transaction(const transaction& other);
    transaction(transaction&& other);

    uint32_t version() const;
    void set_version(uint32_t value);

    uint32_t locktime() const;
    void set_locktime(uint32_t value);

    input::list& inputs();
    const input::list& inputs() const;
    void set_inputs(const input::list& value);
    void set_inputs(input::list&& value);

    output::list& outputs();
    const output::list& outputs() const;
    void set_outputs(const output::list& value);
    void set_outputs(output::list&& value);

    bool is_valid() const;
    bool is_coinbase() const;
    bool is_oversized_coinbase() const;
    bool is_null_non_coinbase() const;
    bool is_overspent() const;
    bool is_final(size_t block_height, uint32_t block_time) const;
    bool is_locktime_conflict() const;
    bool is_missing_inputs() const;
    bool is_immature(size_t target_height) const;
    bool is_double_spend(bool include_unconfirmed) const;

    code check(bool transaction_pool = true) const;
    code accept(const chain_state& state, bool transaction_pool=true) const;
    code connect(const chain_state& state) const;
    code connect_input(const chain_state& state, size_t input_index) const;

    uint64_t total_input_value() const;
    uint64_t total_output_value() const;
    point::indexes missing_inputs() const;
    point::indexes immature_inputs(size_t target_height) const;
    point::indexes double_spends(bool include_unconfirmed) const;
    size_t signature_operations(bool bip16_active) const;
    hash_digest hash(uint32_t sighash_type) const;
    hash_digest hash() const;
    uint64_t fees() const;

    std::string to_string(uint32_t flags) const;

    bool from_data(const data_chunk& data, bool satoshi=true);
    bool from_data(std::istream& stream, bool satoshi=true);
    bool from_data(reader& source, bool satoshi=true);
    data_chunk to_data(bool satoshi=true) const;
    void to_data(std::ostream& stream, bool satoshi=true) const;
    void to_data(writer& sink, bool satoshi=true) const;
    void reset();
    uint64_t serialized_size() const;

    /// This class is move assignable [but not copy assignable].
    transaction& operator=(transaction&& other);
    transaction& operator=(const transaction& other) /* = delete */;

    bool operator==(const transaction& other) const;
    bool operator!=(const transaction& other) const;

    // These fields do not participate in serialization or comparison.
    //-------------------------------------------------------------------------
    mutable metadata metadata;
    mutable validation validation;

private:
    uint32_t version_;
    uint32_t locktime_;
    input::list inputs_;
    output::list outputs_;

    mutable upgrade_mutex hash_mutex_;
    mutable std::shared_ptr<hash_digest> hash_;
};

} // namespace chain
} // namespace libbitcoin

#endif
