/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__MACHINE_PROGRAM_HPP
#define LIBBITCOIN__MACHINE_PROGRAM_HPP

#include <cstdint>
#include <bitcoin/chain_script.hpp>
#include <bitcoin/chain_transaction.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/machine/program.hpp>
#include <bitcoin/machine_number.hpp>
#include <bitcoin/machine_opcode.hpp>
#include <bitcoin/machine_op_iterator.hpp>
#include <bitcoin/machine_operation.hpp>
#include <bitcoin/machine_operation_list.hpp>
#include <bitcoin/machine_script_version.hpp>
#include <bitcoin/machine_stack_iterator.hpp>
#include <bitcoin/p_int32_t.hpp>
#include <bitcoin/utility_data_stack.hpp>

namespace libbitcoin {
//namespace machine {
namespace api {

class BC_API machine_program : public machine::program
{
public:
//    typedef data_stack::value_type value_type;
//    typedef operation::iterator op_iterator;

    // Older libstdc++ does not allow erase with const iterator.
    // This is a bug that requires we up the minimum compiler version.
    // So presently machine_stack_iterator is a non-const iterator.
    ////typedef data_stack::const_iterator machine_stack_iterator;
//    typedef utility_data_stack::iterator stack_iterator;

    /// Create an instance that does not expect to verify signatures.
    /// This is useful for script utilities but not with input metadata.
    /// This can only run individual operations via run(op, program).
    machine_program();

    /// Create an instance that does not expect to verify signatures.
    /// This is useful for script utilities but not with input metadata.
    /// This can run ops via run(op, machine_program) or the script via run(machine_program).
    machine_program(const chain_script& script);

    /// Create an instance with empty stacks, value unused/max (input run).
    machine_program(const chain_script& script, const chain_transaction& transaction,
        uint32_t input_index, uint32_t forks);

    /// Create an instance with initialized stack (witness run, v0 by default).
    machine_program(const chain_script& script, const chain_transaction& transaction,
        uint32_t input_index, uint32_t forks, utility_data_stack&& stack,
        uint64_t value, machine_script_version version=machine_script_version::zero);

    /// Create using copied tx, input, forks, value, stack (prevout run).
    machine_program(const chain_script& script, const machine_program& other);

    /// Create using copied tx, input, forks, value and moved stack (p2sh run).
    machine_program(const chain_script& script, machine_program&& other, bool move);

    /// Constant registers.
    bool is_valid() const;
    uint32_t forks() const;
    uint32_t input_index() const;
    uint64_t value() const;
    machine_script_version version() const;
    const chain_transaction& transaction() const;

    /// Program registers.
    machine_op_iterator begin() const;
    machine_op_iterator jump() const;
    machine_op_iterator end() const;
    size_t operation_count() const;

    /// Instructions.
    error_code evaluate();
    error_code evaluate(const machine_operation& op);
    bool increment_operation_count(const machine_operation& op);
    bool increment_operation_count(int32_t public_keys);
    bool set_jump_register(const machine_operation& op, int32_t offset);

    // Primary stack.
    //-------------------------------------------------------------------------

    /// Primary push.
    void push(bool value);
    void push_move(utility_data_chunk&& item);
    void push_copy(const utility_data_chunk& item);

    /// Primary pop.
    utility_data_chunk pop();
    bool pop(p_int32_t& out_value);
    bool pop(machine_number& out_number, size_t maxiumum_size=max_number_size);
    bool pop_binary(machine_number& first, machine_number& second);
    bool pop_ternary(machine_number& first, machine_number& second, machine_number& third);
    bool pop_position(machine_stack_iterator& out_position);
    bool pop(utility_data_stack& section, size_t count);

    /// Primary push/pop optimizations (active).
    void duplicate(size_t index);
    void swap(size_t index_left, size_t index_right);
    void erase(const machine_stack_iterator& position);
    void erase(const machine_stack_iterator& first, const machine_stack_iterator& last);

    /// Primary push/pop optimizations (passive).
    bool empty() const;
    bool stack_true(bool clean) const;
    bool stack_result(bool clean) const;
    bool is_stack_overflow() const;
    bool if_(const machine_operation& op) const;
    const utility_data_chunk& item(size_t index) /*const*/;
    bool top(machine_number& out_number, size_t maxiumum_size=max_number_size) /*const*/;
    machine_stack_iterator position(size_t index) /*const*/;
    machine_operation_list subscript() const;
    size_t size() const;

    // Alternate stack.
    //-------------------------------------------------------------------------

    bool empty_alternate() const;
    void push_alternate(utility_data_chunk&& value);
    utility_data_chunk pop_alternate();

    // Conditional stack.
    //-------------------------------------------------------------------------

    void open(bool value);
    void negate();
    void close();
    bool closed() const;
    bool succeeded() const;

//private:
//    // A space-efficient dynamic bitset (specialized).
//    typedef std::vector<bool> bool_stack;
//
//    void reserve_stacks();
//    bool stack_to_bool(bool clean) const;
//
//    const chain::script& script_;
//    const chain::transaction& transaction_;
//    const uint32_t input_index_;
//    const uint32_t forks_;
//    const uint64_t value_;
//
//    script_version version_;
//    size_t negative_count_;
//    size_t operation_count_;
//    op_iterator jump_;
//    data_stack primary_;
//    data_stack alternate_;
//    bool_stack condition_;
};

} // namespace api
//} // namespace machine
} // namespace libbitcoin


//#include <bitcoin/bitcoin/impl/machine/program.ipp>

#endif
