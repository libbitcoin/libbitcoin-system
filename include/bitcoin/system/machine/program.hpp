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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/number.hpp>

#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

class BC_API program
{
public:
    typedef chain::operations::const_iterator op_iterator;
    typedef chain::input_cptrs::const_iterator input_iterator;
    typedef std::unordered_map<uint8_t, hash_digest> hash_cache;

    enum class stack
    {
        clean,
        dirty
    };

    /// Input script run (default/empty stack).
    program(const chain::transaction& transaction, const input_iterator& input,
        uint32_t forks) noexcept;

    /// Legacy p2sh or prevout script run (copied input stack).
    program(const program& other, const chain::script::cptr& script) noexcept;

    /// Legacy p2sh or prevout script run (moved input stack).
    program(program&& other, const chain::script::cptr& script) noexcept;

    /// Witness script run (witness-initialized stack).
    program(const chain::transaction& transaction, const input_iterator& input,
        const chain::script::cptr& script, uint32_t forks,
        chain::script_version version, const chunk_cptrs_ptr& stack) noexcept;

    /// Defaults.
    program(program&&) = delete;
    program(const program&) = delete;
    program& operator=(program&&) = delete;
    program& operator=(const program&) = delete;
    ~program() = default;

    /// Program validity.
    bool is_valid() const noexcept;

protected:
    program(const chain::transaction& tx, const input_iterator& input,
        const chain::script::cptr& script, uint32_t forks, uint64_t value,
        chain::script_version version, const chunk_cptrs_ptr& stack) noexcept;

    /// Constants.
    /// -----------------------------------------------------------------------

    inline op_iterator begin() const noexcept
    {
        return script_->ops().begin();
    }

    inline op_iterator end() const noexcept
    {
        return script_->ops().end();
    }

    inline const chain::input& input() const noexcept
    {
        return **input_;
    }

    inline const chain::transaction& transaction() const noexcept
    {
        return transaction_;
    }

    inline bool is_enabled(chain::forks rule) const noexcept
    {
        return to_bool(forks_ & rule);
    }

    /// Primary stack.
    /// -----------------------------------------------------------------------

    /// Primary stack push.

    /// Inherent byte vectors.
    void push(chunk_cptr&& item) noexcept;
    void push(const chunk_cptr& item) noexcept;
    void push_chunk(data_chunk&& item) noexcept;

    /// Inherent integral values.
    void push_bool(bool value) noexcept;
    void push_length(size_t value) noexcept;
    void push_numeric(int64_t value) noexcept;
    void push_number(const number& value) noexcept;

    /// Primary stack pop.

    /// Inherent byte vectors.
    chunk_cptr pop() noexcept;
    bool pop(chunk_cptrs& items, size_t count) noexcept;

    /// Inherent integral values.
    bool pop_index_four_bytes(size_t& out_value) noexcept;
    bool pop_signed_four_bytes(int32_t& out_value) noexcept;
    bool pop_number_four_bytes(number& out_value) noexcept;
    bool pop_number_five_bytes(number& out_value) noexcept;
    bool pop_binary_four_bytes(number& left, number& right) noexcept;
    bool pop_ternary_four_bytes(number& upper, number& lower, number& value) noexcept;
    bool peek_top_unsigned_five_bytes(uint64_t& out_value) const noexcept;

    /// Primary stack optimizations (type-independent).
    void drop() noexcept;
    void swap(size_t left, size_t right) noexcept;
    void erase(size_t index) noexcept;

    /// Primary stack const functions (type-independent).
    size_t size() const noexcept;
    bool is_empty() const noexcept;
    bool is_stack_overflow() const noexcept;
    bool stack_to_bool(stack clean) const noexcept;
    const chunk_cptr& item(size_t index) const noexcept;

    /// Alternate stack.
    /// -----------------------------------------------------------------------

    bool is_alternate_empty() const noexcept;
    void push_alternate(chunk_cptr&& value) noexcept;
    chunk_cptr pop_alternate() noexcept;

    /// Conditional stack.
    /// -----------------------------------------------------------------------

    void begin_if(bool value) noexcept;
    void else_if() noexcept;
    void end_if() noexcept;
    bool is_balanced() const noexcept;
    bool is_succeess() const noexcept;
    bool if_(const chain::operation& op) const noexcept;

    /// Accumulator.
    /// -----------------------------------------------------------------------

    bool ops_increment(const chain::operation& op) noexcept;
    bool ops_increment(size_t public_keys) noexcept;

    /// Signature validation helpers.
    /// -----------------------------------------------------------------------

    /// Set subscript position to next op.
    bool set_subscript(const op_iterator& op) noexcept;

    /// Strip endorsement and code_separator opcodes from returned subscript.
    chain::script::cptr subscript(
        const chunk_cptrs& endorsements) const noexcept;

    /// Prepare signature (enables generalized signing).
    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_digest& hash, const chunk_cptr& endorsement) const noexcept;

    /// Prepare signature, with caching for multisig with same flags.
    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_cache& cache, uint8_t& flags, const data_chunk& endorsement,
        const chain::script& sub) const noexcept;

private:
    // A possibly space-efficient dynamic bitset (specialized by C++ std lib).
    typedef std::vector<bool> bool_stack;

    static chain::operations create_strip_ops(
        const chunk_cptrs& endorsements) noexcept;

    hash_digest signature_hash(const chain::script& sub,
        uint8_t flags) const noexcept;
    void signature_hash(hash_cache& cache, const chain::script& sub,
        uint8_t flags) const noexcept;

    // Constants.
    const chain::transaction& transaction_;
    const input_iterator input_;
    const chain::script::cptr script_;
    const uint32_t forks_;
    const uint64_t value_;
    const chain::script_version version_;

    // Three stacks.
    chunk_cptrs_ptr primary_;
    chunk_cptrs alternate_{};
    bool_stack condition_{};

    // Accumulator.
    size_t operation_count_{};

    // Condition stack optimization.
    size_t negative_condition_count_{};
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
