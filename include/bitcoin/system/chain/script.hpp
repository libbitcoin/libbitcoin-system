/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class transaction;

class BC_API script
{
public:
    typedef std::shared_ptr<const script> cptr;

    /// Utilities.
    /// -----------------------------------------------------------------------

    /// Determine if the flag is enabled in the active flags set.
    static constexpr bool is_enabled(uint32_t active_flags, flags flag) NOEXCEPT;
    static VCONSTEXPR bool is_push_only_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_relaxed_push_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_commitment_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_witness_program_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_null_data_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_op_return_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_multisig_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_public_key_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_key_hash_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_script_hash_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_witness_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_witness_key_hash_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_pay_witness_script_hash_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_sign_multisig_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_sign_public_key_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_sign_key_hash_pattern(const operations& ops) NOEXCEPT;
    static VCONSTEXPR bool is_sign_script_hash_pattern(const operations& ops) NOEXCEPT;
    static bool is_coinbase_pattern(const operations& ops, size_t height) NOEXCEPT;

    static inline operations to_pay_null_data_pattern(
        const data_slice& data) NOEXCEPT;
    static inline operations to_pay_public_key_pattern(
        const data_slice& point) NOEXCEPT;
    static inline operations to_pay_key_hash_pattern(
        const short_hash& hash) NOEXCEPT;
    static inline operations to_pay_script_hash_pattern(
        const short_hash& hash) NOEXCEPT;
    static inline operations to_pay_multisig_pattern(uint8_t signatures,
        const compressed_list& points) NOEXCEPT;
    static inline operations to_pay_multisig_pattern(uint8_t signatures,
        const data_stack& points) NOEXCEPT;
    static inline operations to_pay_witness_pattern(uint8_t version,
        const data_slice& data) NOEXCEPT;
    static inline operations to_pay_witness_key_hash_pattern(
        const short_hash& hash) NOEXCEPT;
    static inline operations to_pay_witness_script_hash_pattern(
        const hash_digest& hash) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default script is an invalid empty script object.
    script() NOEXCEPT;
    virtual ~script() NOEXCEPT;

    /// Metadata is defaulted on copy/assign.
    script(script&& other) NOEXCEPT;
    script(const script& other) NOEXCEPT;
    script(operations&& ops) NOEXCEPT;
    script(const operations& ops) NOEXCEPT;
    script(operations&& ops, bool prefail) NOEXCEPT;
    script(const operations& ops, bool prefail) NOEXCEPT;
    script(const data_slice& data, bool prefix) NOEXCEPT;
    ////script(stream::in::fast&& stream, bool prefix) NOEXCEPT;
    script(stream::in::fast& stream, bool prefix) NOEXCEPT;
    script(std::istream&& stream, bool prefix) NOEXCEPT;
    script(std::istream& stream, bool prefix) NOEXCEPT;
    script(reader&& source, bool prefix) NOEXCEPT;
    script(reader& source, bool prefix) NOEXCEPT;

    // TODO: move to config serialization wrapper.
    script(const std::string& mnemonic) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    /// Metadata is defaulted on copy/assign.
    script& operator=(script&& other) NOEXCEPT;
    script& operator=(const script& other) NOEXCEPT;

    bool operator==(const script& other) const NOEXCEPT;
    bool operator!=(const script& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data(bool prefix) const NOEXCEPT;
    void to_data(std::ostream& stream, bool prefix) const NOEXCEPT;
    void to_data(writer& sink, bool prefix) const NOEXCEPT;

    // TODO: move to config serialization wrapper.
    std::string to_string(uint32_t active_flags) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    bool is_prefail() const NOEXCEPT;
    const operations& ops() const NOEXCEPT;

    /// Computed properties.
    bool is_roller() const NOEXCEPT;
    hash_digest hash() const NOEXCEPT;
    size_t serialized_size(bool prefix) const NOEXCEPT;

    /// Utilities.
    /// -----------------------------------------------------------------------

    /// Pattern optimizations.
    bool is_pay_to_witness(uint32_t active_flags) const NOEXCEPT;
    bool is_pay_to_script_hash(uint32_t active_flags) const NOEXCEPT;

    /// Common pattern detection.
    const data_chunk& witness_program() const NOEXCEPT;
    script_version version() const NOEXCEPT;
    script_pattern pattern() const NOEXCEPT;
    script_pattern input_pattern() const NOEXCEPT;
    script_pattern output_pattern() const NOEXCEPT;

    /// Consensus computations.
    size_t signature_operations(bool accurate) const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;
    bool is_unspendable() const NOEXCEPT;

protected:
    script(operations&& ops, bool valid, bool prefail) NOEXCEPT;
    script(const operations& ops, bool valid, bool prefail) NOEXCEPT;
    script(const operations& ops, bool valid, bool prefail,
        size_t size) NOEXCEPT;

private:
    // TODO: move to config serialization wrapper.
    static script from_string(const std::string& mnemonic) NOEXCEPT;

    static size_t op_count(reader& source) NOEXCEPT;
    static size_t serialized_size(const operations& ops) NOEXCEPT;
    static inline size_t op_size(size_t total, const operation& op) NOEXCEPT
    {
        return ceilinged_add(total, op.serialized_size());
    };

    void assign_data(reader& source, bool prefix) NOEXCEPT;

    // Script should be stored as shared.
    operations ops_;

    // Cache.
    bool valid_;
    bool prefail_;
    size_t size_;

public:
    using iterator = operations::const_iterator;

    /// Public mutable metadata access, not copied or compared for equality.
    mutable iterator offset;
};

typedef std_vector<script> scripts;

DECLARE_JSON_VALUE_CONVERTORS(script);
DECLARE_JSON_VALUE_CONVERTORS(script::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/script.ipp>

namespace std
{
template<>
struct hash<bc::system::chain::script>
{
    size_t operator()(const bc::system::chain::script& value) const NOEXCEPT
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data(false));
    }
};
} // namespace std

#endif
