/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/machine/program.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/machine/interpreter.hpp>
#include <bitcoin/system/machine/script_version.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace bc::system::chain;

// Fixed tuning parameters, max_stack_size ensures no reallocation.
static constexpr size_t stack_capactity = max_stack_size;
static constexpr size_t condition_capactity = max_counted_ops;
static const chain::transaction default_tx_;
static const chain::script default_script_;

void program::reserve_stacks()
{
    primary_.reserve(stack_capactity);
    alternate_.reserve(stack_capactity);
    condition_.reserve(condition_capactity);
}

// Constructors.
//-----------------------------------------------------------------------------

program::program()
  : script_(default_script_),
    transaction_(default_tx_),
    input_index_(0),
    forks_(0),
    value_(0),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
    reserve_stacks();
}

program::program(const script& script)
  : script_(script),
    transaction_(default_tx_),
    input_index_(0),
    forks_(0),
    value_(0),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
    reserve_stacks();
}

program::program(const script& script, const chain::transaction& transaction,
    uint32_t input_index, uint32_t forks)
  : script_(script),
    transaction_(transaction),
    input_index_(input_index),
    forks_(forks),
    value_(max_uint64),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin())
{
    reserve_stacks();
}

// Condition, alternate, jump and operation_count are not copied.
program::program(const script& script, const chain::transaction& transaction,
    uint32_t input_index, uint32_t forks, data_stack&& stack, uint64_t value,
    script_version version)
  : script_(script),
    transaction_(transaction),
    input_index_(input_index),
    forks_(forks),
    value_(value),
    version_(version),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(std::move(stack))
{
    reserve_stacks();
}


// Condition, alternate, jump and operation_count are not copied.
program::program(const script& script, const program& other)
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(other.primary_)
{
    reserve_stacks();
}

// Condition, alternate, jump and operation_count are not moved.
program::program(const script& script, program&& other, bool)
  : script_(script),
    transaction_(other.transaction_),
    input_index_(other.input_index_),
    forks_(other.forks_),
    value_(other.value_),
    version_(script_version::unversioned),
    negative_count_(0),
    operation_count_(0),
    jump_(script_.begin()),
    primary_(std::move(other.primary_))
{
    reserve_stacks();
}

// Instructions.
//-----------------------------------------------------------------------------

code program::evaluate()
{
    return interpreter::run(*this);
}

code program::evaluate(const operation& op)
{
    return interpreter::run(op, *this);
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
