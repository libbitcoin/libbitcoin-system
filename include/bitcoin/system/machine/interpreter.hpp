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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP

#include <cstdint>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/machine/program.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

/// Class to isolate operation iteration, dispatch, and handlers from state.
class BC_API interpreter
  : public program
{
public:
    /// Use program constructors.
    using program::program;

    /// Defaults.
    interpreter(interpreter&&) = delete;
    interpreter(const interpreter&) = delete;
    interpreter& operator=(interpreter&&) = delete;
    interpreter& operator=(const interpreter&) = delete;
    ~interpreter() = default;

    /// Operation handlers.
    error::op_error_t op_unevaluated(chain::opcode) const noexcept;
    error::op_error_t op_nop(chain::opcode) const noexcept;
    error::op_error_t op_push_number(int8_t value) noexcept;
    error::op_error_t op_push_size(const chain::operation& op) noexcept;
    error::op_error_t op_push_one_size(const chain::operation& op) noexcept;
    error::op_error_t op_push_two_size(const chain::operation& op) noexcept;
    error::op_error_t op_push_four_size(const chain::operation& op) noexcept;
    error::op_error_t op_nop() const noexcept;
    error::op_error_t op_ver() const noexcept;
    error::op_error_t op_if() noexcept;
    error::op_error_t op_notif() noexcept;
    error::op_error_t op_verif() const noexcept;
    error::op_error_t op_vernotif() const noexcept;
    error::op_error_t op_else() noexcept;
    error::op_error_t op_endif() noexcept;
    error::op_error_t op_verify() noexcept;
    error::op_error_t op_return() const noexcept;
    error::op_error_t op_to_alt_stack() noexcept;
    error::op_error_t op_from_alt_stack() noexcept;
    error::op_error_t op_drop2() noexcept;
    error::op_error_t op_dup2() noexcept;
    error::op_error_t op_dup3() noexcept;
    error::op_error_t op_over2() noexcept;
    error::op_error_t op_rot2() noexcept;
    error::op_error_t op_swap2() noexcept;
    error::op_error_t op_if_dup() noexcept;
    error::op_error_t op_depth() noexcept;
    error::op_error_t op_drop() noexcept;
    error::op_error_t op_dup() noexcept;
    error::op_error_t op_nip() noexcept;
    error::op_error_t op_over() noexcept;
    error::op_error_t op_pick() noexcept;
    error::op_error_t op_roll() noexcept;
    error::op_error_t op_rot() noexcept;
    error::op_error_t op_swap() noexcept;
    error::op_error_t op_tuck() noexcept;
    error::op_error_t op_cat() const noexcept;
    error::op_error_t op_substr() const noexcept;
    error::op_error_t op_left() const noexcept;
    error::op_error_t op_right() const noexcept;
    error::op_error_t op_size() noexcept;
    error::op_error_t op_invert() const noexcept;
    error::op_error_t op_and() const noexcept;
    error::op_error_t op_or() const noexcept;
    error::op_error_t op_xor() const noexcept;
    error::op_error_t op_equal() noexcept;
    error::op_error_t op_equal_verify() noexcept;
    error::op_error_t op_add1() noexcept;
    error::op_error_t op_sub1() noexcept;
    error::op_error_t op_mul2() const noexcept;
    error::op_error_t op_div2() const noexcept;
    error::op_error_t op_negate() noexcept;
    error::op_error_t op_abs() noexcept;
    error::op_error_t op_not() noexcept;
    error::op_error_t op_nonzero() noexcept;
    error::op_error_t op_add() noexcept;
    error::op_error_t op_sub() noexcept;
    error::op_error_t op_mul() const noexcept;
    error::op_error_t op_div() const noexcept;
    error::op_error_t op_mod() const noexcept;
    error::op_error_t op_lshift() const noexcept;
    error::op_error_t op_rshift() const noexcept;
    error::op_error_t op_bool_and() noexcept;
    error::op_error_t op_bool_or() noexcept;
    error::op_error_t op_num_equal() noexcept;
    error::op_error_t op_num_equal_verify() noexcept;
    error::op_error_t op_num_not_equal() noexcept;
    error::op_error_t op_less_than() noexcept;
    error::op_error_t op_greater_than() noexcept;
    error::op_error_t op_less_than_or_equal() noexcept;
    error::op_error_t op_greater_than_or_equal() noexcept;
    error::op_error_t op_min() noexcept;
    error::op_error_t op_max() noexcept;
    error::op_error_t op_within() noexcept;
    error::op_error_t op_ripemd160() noexcept;
    error::op_error_t op_sha1() noexcept;
    error::op_error_t op_sha256() noexcept;
    error::op_error_t op_hash160() noexcept;
    error::op_error_t op_hash256() noexcept;
    error::op_error_t op_codeseparator(const op_iterator& op) noexcept;
    error::op_error_t op_check_sig_verify() noexcept;
    error::op_error_t op_check_sig() noexcept;
    error::op_error_t op_check_multisig_verify() noexcept;
    error::op_error_t op_check_multisig() noexcept;
    error::op_error_t op_check_locktime_verify() const noexcept;
    error::op_error_t op_check_sequence_verify() const noexcept;

    /// Operation loop (script execution).
    code run() noexcept;

    /// Transaction must pop top input stack element (bip16).
    inline chunk_cptr pop() noexcept
    {
        // Empty guard is not required, but this is a public function.
        BC_ASSERT_MSG(!is_empty(), "pop from empty stack");

        if (is_empty())
            return to_shared<data_chunk>();

        // Avoid ternary object return (prevents copy elision).
        return pop_cptr_unsafe();
    }

protected:
    /// Operation disatch.
    error::op_error_t run_op(const op_iterator& op) noexcept;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
