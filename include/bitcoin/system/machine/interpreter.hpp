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
template <typename Stack>
class interpreter
  : public program<Stack>
{
public:
    using state = typename program<Stack>::program;
    using op_iterator = typename state::op_iterator;
    using input_iterator = typename input_cptrs::const_iterator;

    /// Use program constructors.
    using program<Stack>::program;

    /// Defaults.
    interpreter(interpreter&&) = delete;
    interpreter(const interpreter&) = delete;
    interpreter& operator=(interpreter&&) = delete;
    interpreter& operator=(const interpreter&) = delete;
    inline ~interpreter() = default;

    /// Run a program.
    inline code run() noexcept;

    /// Connect tx.input[#].script to tx.input[#].prevout.script.
    static code connect(const context& state, const transaction& tx,
        uint32_t index) noexcept;

    /// Connect tx.input[*].script to tx.input[*].prevout.script.
    static code connect(const context& state, const transaction& tx,
        const input_iterator& it) noexcept;

protected:
    /// Operation disatch.
    inline error::op_error_t run_op(const op_iterator& op) noexcept;

    /// Operation handlers.
    inline error::op_error_t op_unevaluated(chain::opcode) const noexcept;
    inline error::op_error_t op_nop(chain::opcode) const noexcept;
    inline error::op_error_t op_push_number(int8_t value) noexcept;
    inline error::op_error_t op_push_size(const chain::operation& op) noexcept;
    inline error::op_error_t op_push_one_size(const chain::operation& op) noexcept;
    inline error::op_error_t op_push_two_size(const chain::operation& op) noexcept;
    inline error::op_error_t op_push_four_size(const chain::operation& op) noexcept;
    inline error::op_error_t op_nop() const noexcept;
    inline error::op_error_t op_ver() const noexcept;
    inline error::op_error_t op_if() noexcept;
    inline error::op_error_t op_notif() noexcept;
    inline error::op_error_t op_verif() const noexcept;
    inline error::op_error_t op_vernotif() const noexcept;
    inline error::op_error_t op_else() noexcept;
    inline error::op_error_t op_endif() noexcept;
    inline error::op_error_t op_verify() noexcept;
    inline error::op_error_t op_return() const noexcept;
    inline error::op_error_t op_to_alt_stack() noexcept;
    inline error::op_error_t op_from_alt_stack() noexcept;
    inline error::op_error_t op_drop2() noexcept;
    inline error::op_error_t op_dup2() noexcept;
    inline error::op_error_t op_dup3() noexcept;
    inline error::op_error_t op_over2() noexcept;
    inline error::op_error_t op_rot2() noexcept;
    inline error::op_error_t op_swap2() noexcept;
    inline error::op_error_t op_if_dup() noexcept;
    inline error::op_error_t op_depth() noexcept;
    inline error::op_error_t op_drop() noexcept;
    inline error::op_error_t op_dup() noexcept;
    inline error::op_error_t op_nip() noexcept;
    inline error::op_error_t op_over() noexcept;
    inline error::op_error_t op_pick() noexcept;
    inline error::op_error_t op_roll() noexcept;
    inline error::op_error_t op_rot() noexcept;
    inline error::op_error_t op_swap() noexcept;
    inline error::op_error_t op_tuck() noexcept;
    inline error::op_error_t op_cat() const noexcept;
    inline error::op_error_t op_substr() const noexcept;
    inline error::op_error_t op_left() const noexcept;
    inline error::op_error_t op_right() const noexcept;
    inline error::op_error_t op_size() noexcept;
    inline error::op_error_t op_invert() const noexcept;
    inline error::op_error_t op_and() const noexcept;
    inline error::op_error_t op_or() const noexcept;
    inline error::op_error_t op_xor() const noexcept;
    inline error::op_error_t op_equal() noexcept;
    inline error::op_error_t op_equal_verify() noexcept;
    inline error::op_error_t op_add1() noexcept;
    inline error::op_error_t op_sub1() noexcept;
    inline error::op_error_t op_mul2() const noexcept;
    inline error::op_error_t op_div2() const noexcept;
    inline error::op_error_t op_negate() noexcept;
    inline error::op_error_t op_abs() noexcept;
    inline error::op_error_t op_not() noexcept;
    inline error::op_error_t op_nonzero() noexcept;
    inline error::op_error_t op_add() noexcept;
    inline error::op_error_t op_sub() noexcept;
    inline error::op_error_t op_mul() const noexcept;
    inline error::op_error_t op_div() const noexcept;
    inline error::op_error_t op_mod() const noexcept;
    inline error::op_error_t op_lshift() const noexcept;
    inline error::op_error_t op_rshift() const noexcept;
    inline error::op_error_t op_bool_and() noexcept;
    inline error::op_error_t op_bool_or() noexcept;
    inline error::op_error_t op_num_equal() noexcept;
    inline error::op_error_t op_num_equal_verify() noexcept;
    inline error::op_error_t op_num_not_equal() noexcept;
    inline error::op_error_t op_less_than() noexcept;
    inline error::op_error_t op_greater_than() noexcept;
    inline error::op_error_t op_less_than_or_equal() noexcept;
    inline error::op_error_t op_greater_than_or_equal() noexcept;
    inline error::op_error_t op_min() noexcept;
    inline error::op_error_t op_max() noexcept;
    inline error::op_error_t op_within() noexcept;
    inline error::op_error_t op_ripemd160() noexcept;
    inline error::op_error_t op_sha1() noexcept;
    inline error::op_error_t op_sha256() noexcept;
    inline error::op_error_t op_hash160() noexcept;
    inline error::op_error_t op_hash256() noexcept;
    inline error::op_error_t op_codeseparator(const op_iterator& op) noexcept;
    inline error::op_error_t op_check_sig_verify() noexcept;
    inline error::op_error_t op_check_sig() noexcept;
    inline error::op_error_t op_check_multisig_verify() noexcept;
    inline error::op_error_t op_check_multisig() noexcept;
    inline error::op_error_t op_check_locktime_verify() const noexcept;
    inline error::op_error_t op_check_sequence_verify() const noexcept;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/interpreter.ipp>

#endif
