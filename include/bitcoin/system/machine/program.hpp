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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_HPP

#include <unordered_map>
#include <vector>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/machine/interpreter.hpp>
#include <bitcoin/system/machine/stack.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

/// A set of three stacks (primary, alternate, conditional) for script state.
/// Primary stack is optimized by peekable, swappable, and eraseable elements.
template <typename Stack>
class program
{
public:
    DELETE_COPY_MOVE_DESTRUCT(program);

    typedef chain::operations::const_iterator op_iterator;
    typedef chain::input_cptrs::const_iterator input_iterator;
    typedef std::unordered_map<uint8_t, hash_digest> hash_cache;

    /// Input script run (default/empty stack).
    inline program(const chain::transaction& transaction,
        const input_iterator& input, uint32_t active_flags) NOEXCEPT;

    /// Legacy p2sh or prevout script run (copied input stack).
    inline program(const program& other,
        const chain::script::cptr& script) NOEXCEPT;

    /// Legacy p2sh or prevout script run (moved input stack).
    inline program(program&& other,
        const chain::script::cptr& script) NOEXCEPT;

    /// Witness script run (witness-initialized stack).
    inline program(const chain::transaction& transaction,
        const input_iterator& input, const chain::script::cptr& script,
        uint32_t active_flags, chain::script_version version,
        const chunk_cptrs_ptr& stack) NOEXCEPT;

    /// Program result.
    inline bool is_true(bool clean) const NOEXCEPT;

    /// Transaction must pop top input stack element (bip16).
    inline const data_chunk& pop() NOEXCEPT;

protected:
    INLINE static bool equal_chunks(const stack_variant& left,
        const stack_variant& right) NOEXCEPT;

    /// Constants.
    /// -----------------------------------------------------------------------

    INLINE bool is_prefail() const NOEXCEPT;
    INLINE op_iterator begin() const NOEXCEPT;
    INLINE op_iterator end() const NOEXCEPT;
    INLINE const chain::input& input() const NOEXCEPT;
    INLINE const chain::transaction& transaction() const NOEXCEPT;
    INLINE bool is_enabled(chain::flags flag) const NOEXCEPT;
    INLINE error::script_error_t validate() const NOEXCEPT;

    /// Primary stack.
    /// -----------------------------------------------------------------------
    /// Underscored names reflect lack of guard against empty stack.

    /// Primary stack (push).
    INLINE void push_chunk(data_chunk&& datum) NOEXCEPT;
    INLINE void push_chunk(const chunk_cptr& datum) NOEXCEPT;
    INLINE void push_bool(bool value) NOEXCEPT;
    INLINE void push_signed64(int64_t value) NOEXCEPT;
    INLINE void push_length(size_t value) NOEXCEPT;

    /// Primary stack (pop).
    INLINE chunk_xptr pop_chunk_() NOEXCEPT;
    INLINE bool pop_strict_bool_() NOEXCEPT;
    INLINE bool pop_bool_(bool& value, bool minimal) NOEXCEPT;
    INLINE bool pop_chunks(chunk_xptrs& data, size_t count) NOEXCEPT;
    INLINE bool pop_signed32(int32_t& value) NOEXCEPT;
    INLINE bool pop_signed32_(int32_t& value) NOEXCEPT;
    INLINE bool pop_binary32(int32_t& left, int32_t& right) NOEXCEPT;
    INLINE bool pop_ternary32(int32_t& upper, int32_t& lower, int32_t& value) NOEXCEPT;
    INLINE bool pop_index32(size_t& index) NOEXCEPT;

    /// Primary stack (peek).
    INLINE bool peek_bool_() const NOEXCEPT;
    INLINE bool peek_unsigned32(uint32_t& value) const NOEXCEPT;
    INLINE bool peek_unsigned40(uint64_t& value) const NOEXCEPT;
    INLINE size_t peek_size() const NOEXCEPT;

    /// Primary stack (variant - index).
    INLINE void swap_(size_t left_index, size_t right_index) NOEXCEPT;
    INLINE void erase_(size_t index) NOEXCEPT;
    INLINE const stack_variant& peek_() const NOEXCEPT;
    INLINE const stack_variant& peek_(size_t index) const NOEXCEPT;

    /// Primary stack (variant - top).
    INLINE void drop_() NOEXCEPT;
    INLINE void push_variant(const stack_variant& vary) NOEXCEPT;
    INLINE stack_variant pop_() NOEXCEPT;

    /// Primary stack state (untyped).
    INLINE size_t stack_size() const NOEXCEPT;
    INLINE bool is_stack_empty() const NOEXCEPT;
    INLINE bool is_stack_overflow() const NOEXCEPT;

    /// Alternate stack.
    /// -----------------------------------------------------------------------

    INLINE bool is_alternate_empty() const NOEXCEPT;
    INLINE void push_alternate(stack_variant&& vary) NOEXCEPT;
    INLINE stack_variant pop_alternate_() NOEXCEPT;

    /// Conditional stack.
    /// -----------------------------------------------------------------------

    INLINE void begin_if(bool value) NOEXCEPT;
    INLINE void else_if_() NOEXCEPT;
    INLINE void end_if_() NOEXCEPT;
    INLINE bool is_balanced() const NOEXCEPT;
    INLINE bool is_succeess() const NOEXCEPT;
    INLINE bool if_(const chain::operation& op) const NOEXCEPT;

    /// Accumulator.
    /// -----------------------------------------------------------------------

    INLINE bool ops_increment(const chain::operation& op) NOEXCEPT;
    INLINE bool ops_increment(size_t public_keys) NOEXCEPT;

    /// Signature validation helpers.
    /// -----------------------------------------------------------------------

    /// Set subscript position to next op.
    inline void set_subscript(const op_iterator& op) NOEXCEPT;

    /// Strip endorsement and code_separator opcodes from returned subscript.
    inline chain::script::cptr subscript(
        const chunk_xptrs& endorsements) const NOEXCEPT;

    /// Prepare signature (enables generalized signing).
    inline bool prepare(ec_signature& signature, const data_chunk& key,
        hash_digest& hash, const chunk_xptr& endorsement) const NOEXCEPT;

    /// Prepare signature, with caching for multisig with same sighash flags.
    inline bool prepare(ec_signature& signature, const data_chunk& key,
        hash_cache& cache, uint8_t& sighash_flags, const data_chunk& endorsement,
        const chain::script& sub) const NOEXCEPT;

private:
    using primary_stack = stack<Stack>;

    // Private stack helpers.
    template<size_t Bytes, typename Integer,
        if_signed_integer<Integer> = true,
        if_integral_integer<Integer> = true,
        if_not_greater<Bytes, sizeof(Integer)> = true>
    INLINE bool peek_signed_(Integer& value) const NOEXCEPT;
    INLINE void push_chunk(const chunk_xptr& datum) NOEXCEPT;
    INLINE chunk_xptr peek_chunk_() const NOEXCEPT;
    INLINE bool peek_signed32_(int32_t& value) const NOEXCEPT;
    INLINE bool peek_signed40_(int64_t& value) const NOEXCEPT;
    INLINE bool is_stack_clean() const NOEXCEPT;

    // Signature hashing.
    INLINE hash_digest signature_hash(const chain::script& sub,
        uint8_t flags) const NOEXCEPT;
    INLINE void signature_hash(hash_cache& cache, const chain::script& sub,
        uint8_t flags) const NOEXCEPT;

    // Constants.
    const chain::transaction& transaction_;
    const input_iterator input_;
    const chain::script::cptr script_;
    const uint32_t flags_;
    const uint64_t value_;
    const chain::script_version version_;
    const chunk_cptrs_ptr witness_;

    // Three stacks.
    primary_stack primary_;
    alternate_stack alternate_{};
    condition_stack condition_{};

    // Accumulator.
    size_t operations_{};

    // Condition stack optimization.
    size_t negative_conditions_{};
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Stack>
#define CLASS program<Stack>

#include <bitcoin/system/impl/machine/program.ipp>

#undef CLASS
#undef TEMPLATE

#endif
