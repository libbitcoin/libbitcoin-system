/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class transaction;

class BC_API script
{
public:
    typedef std::shared_ptr<const script> cptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default script is an invalid empty script object.
    script() noexcept;
    ~script() noexcept;

    /// Metadata is defaulted on copy/assign.
    script(script&& other) noexcept;
    script(const script& other) noexcept;

    script(operations&& ops) noexcept;
    script(const operations& ops) noexcept;

    script(const data_slice& data, bool prefix) noexcept;
    script(std::istream&& stream, bool prefix) noexcept;
    script(std::istream& stream, bool prefix) noexcept;
    script(reader&& source, bool prefix) noexcept;
    script(reader& source, bool prefix) noexcept;

    script(const std::string& mnemonic) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    /// Metadata is defaulted on copy/assign.
    script& operator=(script&& other) noexcept;
    script& operator=(const script& other) noexcept;

    bool operator==(const script& other) const noexcept;
    bool operator!=(const script& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const noexcept;
    void to_data(std::ostream& stream, bool prefix) const noexcept;
    void to_data(writer& sink, bool prefix) const noexcept;

    std::string to_string(uint32_t active_forks) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    const operations& ops() const noexcept;

    /// Computed properties.
    hash_digest hash() const noexcept;
    size_t serialized_size(bool prefix) const noexcept;

    // Utilities (static).
    // ------------------------------------------------------------------------

    /// Determine if the fork is enabled in the active forks set.
    static constexpr bool is_enabled(uint32_t active_forks, forks fork) noexcept
    {
        return to_bool(fork & active_forks);
    }

    /// Consensus patterns.
    static bool is_push_only(const operations& ops) noexcept;
    static bool is_relaxed_push(const operations& ops) noexcept;
    static bool is_coinbase_pattern(const operations& ops,
        size_t height) noexcept;
    static bool is_commitment_pattern(const operations& ops) noexcept;
    static bool is_witness_program_pattern(const operations& ops) noexcept;

    /// Common output patterns (psh and pwsh are also consensus).
    static bool is_pay_op_return_pattern(const operations& ops) noexcept;
    static bool is_pay_null_data_pattern(const operations& ops) noexcept;
    static bool is_pay_multisig_pattern(const operations& ops) noexcept;
    static bool is_pay_public_key_pattern(const operations& ops) noexcept;
    static bool is_pay_key_hash_pattern(const operations& ops) noexcept;
    static bool is_pay_script_hash_pattern(const operations& ops) noexcept;
    static bool is_pay_witness_pattern(const operations& ops) noexcept;
    static bool is_pay_witness_key_hash_pattern(
        const operations& ops) noexcept;
    static bool is_pay_witness_script_hash_pattern(
        const operations& ops) noexcept;

    /// Pattern optimizaitons.
    bool is_pay_to_witness(uint32_t forks) const noexcept;
    bool is_pay_to_script_hash(uint32_t forks) const noexcept;

    /// Common input patterns (skh is also consensus).
    static bool is_sign_multisig_pattern(const operations& ops) noexcept;
    static bool is_sign_public_key_pattern(const operations& ops) noexcept;
    static bool is_sign_key_hash_pattern(const operations& ops) noexcept;
    static bool is_sign_script_hash_pattern(const operations& ops) noexcept;

    /// Stack factories.
    static operations to_pay_null_data_pattern(
        const data_slice& data) noexcept;
    static operations to_pay_public_key_pattern(
        const data_slice& point) noexcept;
    static operations to_pay_key_hash_pattern(
        const short_hash& hash) noexcept;
    static operations to_pay_script_hash_pattern(
        const short_hash& hash) noexcept;
    static operations to_pay_multisig_pattern(uint8_t signatures,
        const compressed_list& points) noexcept;
    static operations to_pay_multisig_pattern(uint8_t signatures,
        const data_stack& points) noexcept;
    static  operations to_pay_witness_pattern(uint8_t version,
        const data_slice& data) noexcept;
    static operations to_pay_witness_key_hash_pattern(
        const short_hash& hash) noexcept;
    static operations to_pay_witness_script_hash_pattern(
        const hash_digest& hash) noexcept;

    // Utilities (non-static).
    // ------------------------------------------------------------------------

    /// Common pattern detection.
    const data_chunk& witness_program() const noexcept;
    script_version version() const noexcept;
    script_pattern pattern() const noexcept;
    script_pattern input_pattern() const noexcept;
    script_pattern output_pattern() const noexcept;

    /// Consensus computations.
    size_t sigops(bool accurate) const noexcept;
    bool is_oversized() const noexcept;
    bool is_unspendable() const noexcept;

protected:
    script(operations&& ops, bool valid) noexcept;
    script(const operations& ops, bool valid) noexcept;

private:
    static script from_data(reader& source, bool prefix) noexcept;
    static script from_string(const std::string& mnemonic) noexcept;
    static size_t op_count(reader& source) noexcept;

    // Script should be stored as shared.
    operations ops_;
    bool valid_;

public:
    /// Public mutable metadata access, not copied or compared for equality.
    mutable operations::const_iterator offset;
};

typedef std::vector<script> scripts;

DECLARE_JSON_VALUE_CONVERTORS(script);
DECLARE_JSON_VALUE_CONVERTORS(script::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::script>
{
    size_t operator()(const bc::system::chain::script& value) const noexcept
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data(false));
    }
};
} // namespace std

#endif
