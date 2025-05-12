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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_CONNECT_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_CONNECT_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// TODO: Implement original op_codeseparator concatenation [< 0.3.6].
// TODO: Implement combined script size limit soft fork (20,000) [0.3.6+].

TEMPLATE
code CLASS::
connect(const chain::context& state, const chain::transaction& tx,
    uint32_t index) NOEXCEPT
{
    if (index >= tx.inputs())
        return error::inputs_overflow;

    return connect(state, tx, std::next(tx.inputs_ptr()->begin(), index));
}

TEMPLATE
code CLASS::
connect(const chain::context& state, const chain::transaction& tx,
    const input_iterator& it) NOEXCEPT
{
    using namespace chain;
    const auto& input = **it;
    if (!input.prevout)
        return error::missing_previous_output;

    // Evaluate input script.
    interpreter in_program(tx, it, state.flags);
    if (const auto ec = in_program.run())
        return ec;

    // Evaluate output script using stack copied from input script evaluation.
    const auto& prevout = input.prevout->script_ptr();
    interpreter out_program(in_program, prevout);

    if (auto ec = out_program.run())
    {
        return ec;
    }
    else if (!out_program.is_true(false))
    {
        return error::stack_false;
    }
    else if (prevout->is_pay_to_script_hash(state.flags))
    {
        // Because output script pushed script hash program [bip16].
        if ((ec = connect_embedded(state, tx, it, in_program)))
            return ec;
    }
    else if (prevout->is_pay_to_witness(state.flags))
    {
        // The input script must be empty [bip141].
        if (!input.script().ops().empty())
            return error::dirty_witness;

        // Because output script pushed version and witness program [bip141].
        if ((ec = connect_witness(state, tx, it, *prevout, false)))
            return ec;
    }
    else if (!input.witness().stack().empty())
    {
        // A non-witness program must have empty witness field [bip141].
        return error::unexpected_witness;
    }

    return error::script_success;
}

// static/protected
TEMPLATE
code CLASS::connect_embedded(const chain::context& state,
    const chain::transaction& tx, const input_iterator& it,
    interpreter& in_program) NOEXCEPT
{
    using namespace chain;
    const auto& input = **it;

    // Input script is limited to relaxed push data operations [bip16].
    if (!script::is_relaxed_push_pattern(input.script().ops()))
        return error::invalid_script_embed;

    // Embedded script must be at the top of the stack [bip16].
    // Evaluate embedded script using stack moved from input script.
    const auto embedded = to_shared<script>(in_program.pop(), false);
    interpreter out_program(std::move(in_program), embedded);

    if (auto ec = out_program.run())
    {
        return ec;
    }
    else if (!out_program.is_true(false))
    {
        return error::stack_false;
    }
    else if (embedded->is_pay_to_witness(state.flags))
    {
        // The input script must be a push of the embedded_script [bip141].
        if (input.script().ops().size() != one)
            return error::dirty_witness;

        // Because output script pushed version/witness program [bip141].
        if ((ec = connect_witness(state, tx, it, *embedded, true)))
            return ec;
    }
    else if (!input.witness().stack().empty())
    {
        // A non-witness program must have empty witness field [bip141].
        return error::unexpected_witness;
    }

    return error::script_success;
}

// static/protected
TEMPLATE
code CLASS::connect_witness(const chain::context& state,
    const chain::transaction& tx, const input_iterator& it,
    const chain::script& prevout, bool embedded) NOEXCEPT
{
    using namespace chain;
    const auto& input = **it;
    const auto flags = state.flags;
    const auto version = prevout.version();
    code ec;

    switch (version)
    {
        case script_version::segwit:
        {
            script::cptr script;
            chunk_cptrs_ptr stack;
            if ((ec = input.witness().extract_script(script, stack, prevout)))
                return ec;

            interpreter program(tx, it, script, flags, version, stack);

            if ((ec = program.run()))
            {
                return ec;
            }
            else if (!program.is_true(true))
            {
                // A v0 script must succeed with a clean true stack [bip141].
                return error::stack_false;
            }
            else
            {
                return error::script_success;
            }
        }

        case script_version::taproot:
        {
            const auto bip341 = script::is_enabled(flags, flags::bip341_rule);

            // Behaves as script_version::reserved if taproot not active.
            // P2SH-wrapped version 1 outputs remain unencumbered [bip341].
            if (!bip341 || embedded)
                return error::script_success;

            script::cptr script;
            chunk_cptrs_ptr stack;
            if ((ec = input.witness().extract_script(script, stack, prevout)))
                return ec;

            interpreter program(tx, it, script, flags, version, stack, {});

            if ((ec = program.run()))
            {
                return ec;
            }
            else if (!program.is_true(true))
            {
                // A v1 script must succeed with a clean true stack [bip342].
                return error::stack_false;
            }
            else
            {
                return error::script_success;
            }
        }

        // These versions are reserved for future extensions [bip141].
        case script_version::reserved:
            return error::script_success;

        case script_version::unversioned:
        default:
            return error::unversioned_script;
    }
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
