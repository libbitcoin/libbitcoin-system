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
#include <bitcoin/system/machine/interpreter.hpp>

#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/machine/operation.hpp>
#include <bitcoin/system/machine/program.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

code interpreter::run(program& program)
{
    code ec;

    if (!program.is_valid())
        return error::invalid_script;

    for (const auto& op: program)
    {
        if (op.is_oversized())
            return error::invalid_push_data_size;

        if (op.is_disabled())
            return error::op_disabled;

        if (!program.increment_operation_count(op))
            return error::invalid_operation_count;

        if (program.if_(op))
        {
            if ((ec = run_op(op, program)))
                return ec;

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

} // namespace machine
} // namespace system
} // namespace libbitcoin
