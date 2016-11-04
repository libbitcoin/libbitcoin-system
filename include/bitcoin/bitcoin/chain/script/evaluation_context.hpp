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
#include <bitcoin/bitcoin/chain/script/conditional_stack.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace chain {

// All index parameters are zero-based and relative to stack top.
class BC_API script;

class evaluation_context
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
    evaluation_context(uint32_t flags);

    /// Create using copied flags and moved stack (only).
    evaluation_context(evaluation_context&& other);

    /// Create using copied flags and copied stack (only).
    evaluation_context(const evaluation_context& other);

    /// Instructions.
    bool set_script(const script& script);
    void set_jump(op_iterator instruction);

    /// Operation count.
    bool update_operation_count(const operation& op);
    bool update_pubkey_count(int32_t multisig_pubkeys);

    /// Properties.
    op_iterator begin() const;
    op_iterator jump() const;
    op_iterator end() const;
    uint32_t flags() const;

    /// Stack info.
    script subscript() const;
    const value_type& item(size_t index) /*const*/;
    stack_iterator position(size_t index) /*const*/;
    bool is_short_circuited(const operation& op) const;
    bool is_stack_overflow() const;
    bool stack_state() const;
    bool stack_result() const;
    bool empty() const;
    size_t size() const;

    /// Stack pop.
    data_chunk pop();
    bool pop(data_stack& section, size_t count);
    bool pop(int32_t& out_value);
    bool pop(number& out_number, size_t maxiumum_size=max_number_size);
    bool pop_binary(number& first, number& second);
    bool pop_ternary(number& first, number& second, number& third);
    bool pop_position(stack_iterator& out_position);
    void erase(const stack_iterator& position);
    void erase(const stack_iterator& first, const stack_iterator& last);

    /// Stack push.
    void push(bool value);
    void push_move(value_type&& item);
    void push_copy(const value_type& item);
    void duplicate(size_t index);
    void swap(size_t index_left, size_t index_right);

    /// Alternate stack.
    bool empty_alternate() const;
    value_type pop_alternate();
    void push_alternate(value_type&& value);

    /// Conditional stack.
    void close();
    void negate();
    void open(bool value);
    bool closed() const;
    bool succeeded() const;

private:
    bool stack_to_bool() const;

    data_stack stack_;
    data_stack alternate_;
    conditional_stack condition_;

    op_iterator begin_;
    op_iterator jump_;
    op_iterator end_;

    const uint32_t flags_;
    size_t op_count_;
};

} // namespace chain
} // namespace libbitcoin

#endif
