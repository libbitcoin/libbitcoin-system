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
#include <bitcoin/system/error/script_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(script_error)
{
    { script_success, "script success" },
    { inputs_overflow, "index exceeds inputs" },
    { prefail_script, "script contains invalid opcode" },
    { prevalid_script, "script contains success opcode" },
    { invalid_script, "invalid script" },
    { invalid_script_size, "invalid script size" },
    { invalid_push_data_size, "invalid push data size" },
    { invalid_operation_count, "invalid operation count" },
    { invalid_stack_size, "invalid stack size" },
    { invalid_stack_scope, "invalid stack scope" },
    { invalid_script_embed, "invalid script embed" },
    { unversioned_script, "unversioned script" },
    { unexpected_witness, "unexpected witness" },
    { invalid_witness, "invalid witness" },
    { invalid_witness_stack, "invalid witness stack" },
    { invalid_commitment, "invalid tapscript commitment" },
    { dirty_witness, "dirty witness" },
    { stack_false, "stack false" }
};

DEFINE_ERROR_T_CATEGORY(script_error, "script", "script code")

} // namespace error
} // namespace system
} // namespace libbitcoin
