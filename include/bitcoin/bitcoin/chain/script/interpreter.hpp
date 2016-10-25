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
#ifndef LIBBITCOIN_CHAIN_INTERPRETER_HPP
#define LIBBITCOIN_CHAIN_INTERPRETER_HPP

#include <cstdint>
#include <bitcoin/bitcoin/chain/script/evaluation_context.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction;

class BC_API interpreter
{
public:
    static bool run(const transaction& tx, uint32_t input_index,
        const script& script, evaluation_context& context, uint32_t flags);

private:
    static bool next_operation(const transaction& tx, uint32_t input_index,
        operation::stack::const_iterator it, const script& script,
        evaluation_context& context, uint32_t flags);

    static bool run_operation(const operation& op, const transaction& tx,
        uint32_t input_index, const script& script,
        evaluation_context& context, uint32_t flags);
};

data_chunk bool_to_stack(bool value);
bool stack_to_bool(const data_chunk& values);
bool stack_result(const evaluation_context& context);

} // namespace chain
} // namespace libbitcoin

#endif
