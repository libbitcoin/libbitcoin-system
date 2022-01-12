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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP

#include <cstddef>
#include <cstdint>
#include <map>
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
    typedef chunk_ptrs::const_iterator stack_iterator;
    typedef std::map<uint8_t, hash_digest> hash_cache;

    /// Create an instance with empty stacks, value unused/max (input run).
    program(const chain::script::ptr& script,
        const chain::transaction& transaction, uint32_t index,
        uint32_t forks) noexcept;

    /// Create an instance with initialized stack (witness run, v0 by default).
    program(const chain::script::ptr& script,
        const chain::transaction& transaction,
        uint32_t index, uint32_t forks, chunk_ptrs&& stack, uint64_t value,
        chain::script_version version=chain::script_version::zero) noexcept;

    /// Create using copied tx, input, forks, value, stack (prevout run).
    program(const chain::script::ptr& script, const program& other) noexcept;

    /// Create using copied tx, input, forks, value and moved stack (p2sh run).
    program(const chain::script::ptr& script, program&& other) noexcept;

    /// Utilities.
    bool is_invalid() const noexcept;
    bool is_enabled(chain::forks rule) const noexcept;

    /// Constant registers.
    const chain::input& input() const noexcept;
    const chain::transaction& transaction() const noexcept;

    /// Program registers.
    op_iterator begin() const noexcept;
    op_iterator end() const noexcept;

    /// Count registers.
    bool increment_op_count(const chain::operation& op) noexcept;
    bool increment_op_count(int32_t public_keys) noexcept;

    // Primary stack.
    // ------------------------------------------------------------------------

    /// Primary push.
    void push(bool value) noexcept;
    void push_move(data_chunk&& item) noexcept;
    void push_copy(const data_chunk& item) noexcept;
    void push_ref(chunk_ptr&& item) noexcept;
    void push_ref(const chunk_ptr& item) noexcept;

    /// Primary pop.
    data_chunk pop() noexcept;
    chunk_ptr pop_ref() noexcept;
    bool pop(int32_t& out_value) noexcept;
    bool pop(number& out_number,
        size_t maxiumum_size=chain::max_number_size) noexcept;
    bool pop_binary(number& first, number& second) noexcept;
    bool pop_ternary(number& first, number& second, number& third) noexcept;
    bool pop_position(stack_iterator& out_position) noexcept;
    bool pop(chunk_ptrs& section, int32_t signed_count) noexcept;

    /// Primary push/pop optimizations (active).
    void duplicate(size_t index) noexcept;
    void swap(size_t left, size_t right) noexcept;
    void erase(const stack_iterator& position) noexcept;
    void erase(const stack_iterator& first, const stack_iterator& last) noexcept;

    /// Primary push/pop optimizations (passive).
    size_t size() const noexcept;
    bool empty() const noexcept;
    bool stack_true(bool clean) const noexcept;
    bool stack_result(bool clean) const noexcept;
    bool is_stack_overflow() const noexcept;
    bool if_(const chain::operation& op) const noexcept;
    bool top(number& out_number,
        size_t maxiumum_size=chain::max_number_size) const noexcept;
    stack_iterator position(size_t index) const noexcept;
    chunk_ptr item(size_t index) const noexcept;

    // Alternate stack.
    // ------------------------------------------------------------------------

    bool empty_alternate() const noexcept;
    void push_alternate(chunk_ptr&& value) noexcept;
    chunk_ptr pop_alternate() noexcept;

    // Conditional stack.
    // ------------------------------------------------------------------------

    void open(bool value) noexcept;
    void negate() noexcept;
    void close() noexcept;
    bool closed() const noexcept;
    bool succeeded() const noexcept;

    // Signature validation.
    // ------------------------------------------------------------------------

    /// Set subscript position to next op.
    bool set_subscript(const chain::operation& op) noexcept;

    /// Parameterized overload strips opcodes from returned subscript.
    chain::script::ptr subscript(const chunk_ptrs& endorsements) const noexcept;

    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_digest& hash, const chunk_ptr& endorsement) const noexcept;

    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_cache& cache, uint8_t& flags, const data_chunk& endorsement,
        const chain::script& sub) const noexcept;

private:
    // A space-efficient dynamic bitset (specialized by c++ std lib).
    typedef std::vector<bool> bool_stack;

    static chain::operations create_strip_ops(
        const chunk_ptrs& endorsements) noexcept;

    hash_digest signature_hash(const chain::script& sub,
        uint8_t flags) const noexcept;
    void signature_hash(hash_cache& cache, const chain::script& sub,
        uint8_t flags) const noexcept;

    bool stack_to_bool(bool clean) const noexcept;

    const chain::script::ptr script_;
    const chain::transaction& transaction_;
    const uint32_t input_index_;
    const uint32_t forks_;
    const uint64_t value_;
    const chain::script_version version_;

    size_t negative_count_;
    size_t operation_count_;
    chunk_ptrs primary_;
    chunk_ptrs alternate_;
    bool_stack condition_;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
