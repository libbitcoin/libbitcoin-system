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
#ifndef LIBBITCOIN_CHAIN_EVALUATION_CONTEXT_HPP
#define LIBBITCOIN_CHAIN_EVALUATION_CONTEXT_HPP

#include <cstdint>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace chain {

// All index parameters are zero-based and relative to stack top.
class script;
class transaction;

class BC_API evaluation_context
{
public:
    typedef script_number number;
    typedef data_stack::value_type value_type;
    typedef operation::const_iterator op_iterator;

    // Older libstdc++ does not allow erase with const iterator.
    // This is a bug that requires we up the minimum compiler version.
    // So presently stack_iterator is a non-const iterator.
    ////typedef data_stack::const_iterator stack_iterator;
    typedef data_stack::iterator stack_iterator;

    /// Create an instance with empty stacks.
    evaluation_context(const chain::transaction& transaction,
        uint32_t input_index, uint32_t flags);

    /// Create using copied flags and copied stack (only).
    evaluation_context(const evaluation_context& other);

    /// Create using copied flags and moved stack (only).
    evaluation_context(evaluation_context&& other, bool move);

    /// Program counter.
    op_iterator begin() const;
    op_iterator jump() const;
    op_iterator end() const;

    /// Instructions.
    bool set_program_counter(const script& script);
    void set_jump_register(op_iterator instruction);

    /// Operation count.
    bool update_operation_count(const operation& op);
    bool update_multisig_public_key_count(int32_t count);

    /// Properties.
    uint32_t flags() const;
    uint32_t input_index() const;
    const chain::transaction& transaction() const;

    // Primary stack.
    //-------------------------------------------------------------------------

    /// Primary push.
    void push(bool value);
    void push_move(value_type&& item);
    void push_copy(const value_type& item);

    /// Primary pop.
    data_chunk pop();
    bool pop(int32_t& out_value);
    bool pop(number& out_number, size_t maxiumum_size=max_number_size);
    bool pop_binary(number& first, number& second);
    bool pop_ternary(number& first, number& second, number& third);
    bool pop_position(stack_iterator& out_position);
    bool pop(data_stack& section, size_t count);

    /// Primary push/pop optimizations (active).
    void duplicate(size_t index);
    void swap(size_t index_left, size_t index_right);
    void erase(const stack_iterator& position);
    void erase(const stack_iterator& first, const stack_iterator& last);

    /// Primary push/pop optimizations (passive).
    bool empty() const;
    bool stack_true() const;
    bool stack_false() const;
    bool is_stack_overflow() const;
    bool is_short_circuited(const operation& op) const;
    const value_type& item(size_t index) /*const*/;
    stack_iterator position(size_t index) /*const*/;
    script subscript() const;
    size_t size() const;

    // Alternate stack.
    //-------------------------------------------------------------------------

    bool empty_alternate() const;
    void push_alternate(value_type&& value);
    value_type pop_alternate();

    // Conditional stack.
    //-------------------------------------------------------------------------

    void open(bool value);
    void negate();
    void close();
    bool closed() const;
    bool succeeded() const;

private:
    typedef std::vector<bool> bool_stack;

    bool stack_to_bool() const;

    op_iterator begin_;
    op_iterator jump_;
    op_iterator end_;

    data_stack primary_;
    data_stack alternate_;
    bool_stack condition_;
    size_t operation_count_;

    const uint32_t flags_;
    const uint32_t input_index_;
    const chain::transaction& transaction_;
};

} // namespace chain
} // namespace libbitcoin

#endif
