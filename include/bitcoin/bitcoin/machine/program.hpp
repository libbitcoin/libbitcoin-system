/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MACHINE_PROGRAM_HPP
#define LIBBITCOIN_MACHINE_PROGRAM_HPP

#include <cstdint>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/machine/number.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace machine {

class BC_API program
{
public:
    typedef data_stack::value_type value_type;
    typedef operation::iterator op_iterator;

    // Older libstdc++ does not allow erase with const iterator.
    // This is a bug that requires we up the minimum compiler version.
    // So presently stack_iterator is a non-const iterator.
    ////typedef data_stack::const_iterator stack_iterator;
    typedef data_stack::iterator stack_iterator;

    /// Create an instance that does not expect to verify signatures.
    /// This is useful for script utilities but not with input validation.
    /// This can only run individual operations via run(op, program).
    program();

    /// Create an instance that does not expect to verify signatures.
    /// This is useful for script utilities but not with input validation.
    /// This can run ops via run(op, program) or the script via run(program).
    program(const chain::script& script);

    /// Create an instance with empty stacks (input run).
    program(const chain::script& script, const chain::transaction& transaction,
        uint32_t input_index, uint32_t forks);

    /// Create using copied forks and copied stack (prevout run).
    program(const chain::script& script, const program& other);

    /// Create using copied forks and moved stack (p2sh run).
    program(const chain::script& script, program&& other, bool move);

    /// Constant registers.
    inline bool is_valid() const;
    inline uint32_t forks() const;
    inline uint32_t input_index() const;
    inline const chain::transaction& transaction() const;

    /// Program registers.
    inline op_iterator begin() const;
    inline op_iterator jump() const;
    inline op_iterator end() const;
    inline size_t operation_count() const;

    /// Instructions.
    code evaluate();
    code evaluate(const operation& op);
    inline bool increment_operation_count(const operation& op);
    inline bool increment_multisig_public_key_count(int32_t count);
    inline bool set_jump_register(const operation& op, int32_t offset);

    // Primary stack.
    //-------------------------------------------------------------------------

    /// Primary push.
    inline void push(bool value);
    inline void push_move(value_type&& item);
    inline void push_copy(const value_type& item);

    /// Primary pop.
    inline data_chunk pop();
    inline bool pop(int32_t& out_value);
    inline bool pop(number& out_number, size_t maxiumum_size=max_number_size);
    inline bool pop_binary(number& first, number& second);
    inline bool pop_ternary(number& first, number& second, number& third);
    inline bool pop_position(stack_iterator& out_position);
    inline bool pop(data_stack& section, size_t count);

    /// Primary push/pop optimizations (active).
    inline void duplicate(size_t index);
    inline void swap(size_t index_left, size_t index_right);
    inline void erase(const stack_iterator& position);
    inline void erase(const stack_iterator& first, const stack_iterator& last);

    /// Primary push/pop optimizations (passive).
    inline bool empty() const;
    inline bool stack_true() const;
    inline bool stack_result() const;
    inline bool is_stack_overflow() const;
    inline bool if_(const operation& op) const;
    inline const value_type& item(size_t index) /*const*/;
    inline bool top(number& out_number, size_t maxiumum_size=max_number_size) /*const*/;
    inline stack_iterator position(size_t index) /*const*/;
    inline operation::list subscript() const;
    inline size_t size() const;

    // Alternate stack.
    //-------------------------------------------------------------------------

    inline bool empty_alternate() const;
    inline void push_alternate(value_type&& value);
    inline value_type pop_alternate();

    // Conditional stack.
    //-------------------------------------------------------------------------

    inline void open(bool value);
    inline void negate();
    inline void close();
    inline bool closed() const;
    inline bool succeeded() const;

private:
    // A space-efficient dynamic bitset (specialized).
    typedef std::vector<bool> bool_stack;

    void reserve_stacks();
    inline bool stack_to_bool() const;

    const chain::script& script_;
    const chain::transaction& transaction_;
    const uint32_t input_index_;
    const uint32_t forks_;

    size_t operation_count_;
    op_iterator jump_;
    data_stack primary_;
    data_stack alternate_;
    bool_stack condition_;
};

} // namespace machine
} // namespace libbitcoin


#include <bitcoin/bitcoin/impl/machine/program.ipp>

#endif
