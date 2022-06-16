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
#include <iterator>
////#include <list>
#include <unordered_map>
#include <variant>
#include <vector>
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

/// A set of three stacks (primary, alternate, conditional) for script state.
/// Primary stack is optimized by peekable, swappable, and eraseable elements.
class BC_API program
{
public:
    typedef chain::operations::const_iterator op_iterator;
    typedef chain::input_cptrs::const_iterator input_iterator;
    typedef std::unordered_map<uint8_t, hash_digest> hash_cache;

    typedef std::variant<bool, int64_t, chunk_xptr> variant;
    typedef std::vector<variant> variant_stack;
    typedef std::shared_ptr<variant_stack> variant_stack_ptr;

    template<class... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;
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
    error::script_error_t validate() const noexcept;

    /// Program result.
    bool is_true(bool clean) const noexcept;

protected:
    /// Constants.
    /// -----------------------------------------------------------------------

    inline bool is_prefail() const noexcept
    {
        return script_->is_prefail();
    }

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

    /// Primary stack (push).
    void push_chunk(data_chunk&& datum) noexcept;
    void push_chunk(const chunk_cptr& datum) noexcept;
    void push_bool(bool value) noexcept;
    void push_signed64(int64_t value) noexcept;
    void push_length(size_t value) noexcept;

    /// Primary stack (pop).
    chunk_xptr pop_chunk_unsafe() noexcept;
    bool pop_bool_unsafe() noexcept;
    bool pop_strict_bool_unsafe() noexcept;
    bool pop_chunks(chunk_xptrs& data, size_t count) noexcept;
    bool pop_signed32(int32_t& value) noexcept;
    bool pop_binary32(int32_t& left, int32_t& right) noexcept;
    bool pop_ternary32(int32_t& upper, int32_t& lower, int32_t& value) noexcept;
    bool pop_index32(size_t& index) noexcept;

    /// Primary stack (peek).
    bool peek_bool_unsafe() const noexcept;
    bool peek_unsigned32(uint32_t& value) const noexcept;
    bool peek_unsigned40(uint64_t& value) const noexcept;

    /// Primary stack (variant).
    void drop_unsafe() noexcept;
    void swap_unsafe(size_t left_index, size_t right_index) noexcept;
    void erase_unsafe(size_t index) noexcept;
    void push_variant(const variant& vary) noexcept;
    variant pop_variant_unsafe() noexcept;
    const variant& peek_variant_unsafe() const noexcept;
    const variant& peek_variant_unsafe(size_t peek_index) const noexcept;

    /// Primary stack state (untyped).
    size_t size() const noexcept;
    bool is_empty() const noexcept;
    bool is_overflow() const noexcept;

    /// Alternate stack.
    /// -----------------------------------------------------------------------

    bool is_alternate_empty() const noexcept;
    void push_alternate(variant&& vary) noexcept;
    variant pop_alternate_unsafe() noexcept;

    /// Conditional stack.
    /// -----------------------------------------------------------------------

    void begin_if(bool value) noexcept;
    void else_if_unsafe() noexcept;
    void end_if_unsafe() noexcept;
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
        const chunk_xptrs& endorsements) const noexcept;

    /// Prepare signature (enables generalized signing).
    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_digest& hash, const chunk_xptr& endorsement) const noexcept;

    /// Prepare signature, with caching for multisig with same flags.
    bool prepare(ec_signature& signature, const data_chunk& key,
        hash_cache& cache, uint8_t& flags, const data_chunk& endorsement,
        const chain::script& sub) const noexcept;

private:
    // A possibly space-efficient dynamic bitset (specialized by C++ std lib).
    typedef std::vector<bool> bool_stack;

    // Private stack helpers.
    template<size_t Bytes, typename Integer,
        if_signed_integer<Integer> = true,
        if_integral_integer<Integer> = true,
        if_not_greater<Bytes, sizeof(Integer)> = true>
    bool peek_signed_unsafe(Integer& value) const noexcept;
    void push_chunk(const chunk_xptr& datum) noexcept;
    bool pop_signed32_unsafe(int32_t& value) noexcept;
    chunk_xptr peek_chunk_unsafe() const noexcept;
    bool peek_strict_bool_unsafe() const noexcept;
    bool peek_signed32_unsafe(int32_t& value) const noexcept;
    bool peek_signed40_unsafe(int64_t& value) const noexcept;
    bool is_clean() const noexcept;

    // Signature hashing.
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
    const chunk_cptrs_ptr witness_;

    // Mutable as this may be updated on peeks.
    mutable tether<data_chunk> tether_;

    // Three stacks.
    variant_stack primary_;
    std::vector<variant> alternate_{};
    bool_stack condition_{};

    // Accumulator.
    size_t operation_count_{};

    // Condition stack optimization.
    size_t negative_condition_count_{};
};

/// Variant comparison overloaded for type conversions.
bool operator==(const program::variant& left,
    const program::variant& right) noexcept;

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
