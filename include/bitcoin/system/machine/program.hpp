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

namespace libbitcoin {
namespace system {
namespace machine {

class BC_API program
{
public:
    typedef chain::operations::const_iterator op_iterator;
    typedef std::unordered_map<uint8_t, hash_digest> hash_cache;

    enum class stack
    {
        clean,
        dirty
    };

    /// Create an instance with empty stacks, value unused/max (input run).
    program(const chain::script::cptr& script,
        const chain::transaction& transaction,
        uint32_t index,
        uint32_t forks) noexcept;

    /// Create using copied tx, input, forks, value, stack (prevout run).
    program(const chain::script::cptr& script,
        const program& other) noexcept;

    /// Create using copied tx, input, forks, value and moved stack (p2sh run).
    program(const chain::script::cptr& script,
        program&& other) noexcept;

    /// Create an instance with initialized stack (witness run).
    program(const chain::script::cptr& script,
        const chain::transaction& transaction,
        uint32_t index,
        uint32_t forks,
        chunk_cptrs&& stack,
        uint64_t value,
        chain::script_version version) noexcept;

    /// Defaults.
    program(program&&) = delete;
    program(const program&) = delete;
    program& operator=(program&&) = delete;
    program& operator=(const program&) = delete;
    ~program() = default;

    /// Utilities.
    bool is_invalid() const noexcept;
    bool is_enabled(chain::forks rule) const noexcept;
    bool is_stack_true(stack clean) const noexcept;

    /// Accumulator.
    bool ops_increment(const chain::operation& op) noexcept;
    bool ops_increment(int32_t public_keys) noexcept;

    /// Program registers.
    op_iterator begin() const noexcept;
    op_iterator end() const noexcept;

    /// Pop an element from the stack (empty if stack is empty).
    inline chunk_cptr safe_pop() noexcept
    {
        return is_empty() ? to_shared<data_chunk>() : pop();
    }

protected:
    friend class interpreter;

    /// Constant registers.
    /// -----------------------------------------------------------------------

    const chain::input& input() const noexcept;
    const chain::transaction& transaction() const noexcept;

    /// Primary stack.
    /// -----------------------------------------------------------------------

    /// Primary stack push.
    void push(bool value) noexcept;
    void push(data_chunk&& item) noexcept;
    void push(chunk_cptr&& item) noexcept;
    void push(const chunk_cptr& item) noexcept;

    /// Primary stack pop.
    chunk_cptr pop() noexcept;
    bool pop(int32_t& out_value) noexcept;
    bool pop(number& out_number,
        size_t maxiumum_size=chain::max_number_size) noexcept;
    bool pop_index(size_t& index) noexcept;
    bool pop_binary(number& left, number& right) noexcept;
    bool pop_ternary(number& upper, number& lower, number& value) noexcept;
    bool pop(chunk_cptrs& section, int32_t signed_count) noexcept;

    /// Primary stack push/pop optimizations.
    void drop() noexcept;
    void swap(size_t left, size_t right) noexcept;
    void erase(size_t index) noexcept;

    /// Primary stack push/pop const functions.
    size_t size() const noexcept;
    bool is_empty() const noexcept;
    bool is_stack_overflow() const noexcept;
    bool get_top(number& out_number, size_t maxiumum_size) const noexcept;
    const chunk_cptr& item(size_t index) const noexcept;

    /// Alternate stack.
    /// -----------------------------------------------------------------------

    bool is_alternate_empty() const noexcept;
    void push_alternate(chunk_cptr&& value) noexcept;
    chunk_cptr pop_alternate() noexcept;

    /// Conditional stack.
    /// -----------------------------------------------------------------------

    void open(bool value) noexcept;
    void negate() noexcept;
    void close() noexcept;
    bool is_closed() const noexcept;
    bool is_succeess() const noexcept;
    bool if_(const chain::operation& op) const noexcept;

    /// Signature validation helpers.
    /// -----------------------------------------------------------------------

    /// Set subscript position to next op.
    bool set_subscript(const chain::operation& op) noexcept;

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
    // A space-efficient dynamic bitset (specialized by C++ std lib).
    typedef std::vector<bool> bool_stack;

    static chain::operations create_strip_ops(
        const chunk_cptrs& endorsements) noexcept;

    bool stack_to_bool(stack clean) const noexcept;
    hash_digest signature_hash(const chain::script& sub,
        uint8_t flags) const noexcept;
    void signature_hash(hash_cache& cache, const chain::script& sub,
        uint8_t flags) const noexcept;

    const chain::script::cptr script_;
    const chain::transaction& transaction_;
    const uint32_t input_index_;
    const uint32_t forks_;
    const uint64_t value_;
    const chain::script_version version_;

    size_t negative_count_;
    size_t operation_count_;
    chunk_cptrs primary_;
    chunk_cptrs alternate_;
    bool_stack condition_;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
