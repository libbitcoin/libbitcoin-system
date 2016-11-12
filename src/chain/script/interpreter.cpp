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
#include <bitcoin/bitcoin/chain/script/interpreter.hpp>

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/program.hpp>
#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
namespace chain {

// TODO: inline all program methods.
// TODO: inline all number methods.
// TODO: inline operation methods used by interpreter and program.
code interpreter::run(program& program)
{
    result error(error::invalid_script);

    if (!program.is_valid())
        return error;

    for (const auto& op: program)
    {
        if (op.is_oversized())
            return error::invalid_push_data_size;

        if (op.is_disabled())
            return run(op, program);

        if (!program.increment_operation_count(op))
            return error::invalid_operation_count;

        if (program.if_(op))
        {
            if ((error = run_op(op, program)))
                return error;

            if (program.is_stack_overflow())
                return error::invalid_stack_size;
        }
    }

    return program.closed() ? error::success : error::invalid_stack_scope;
}

code interpreter::run(const operation& op, program& program)
{
    return run_op(op, program);
}

} // namespace chain
} // namespace libbitcoin
