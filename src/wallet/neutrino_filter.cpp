/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/wallet/neutrino_filter.hpp>

#include <algorithm>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace neutrino {

// Golomb-Rice related values (bip158).
constexpr uint8_t golomb_bits = 19;
constexpr uint64_t golomb_target_false_positive_rate = 784931;

constexpr auto rate = golomb_target_false_positive_rate;

bool compute_filter(const chain::block& block, data_chunk& out_filter) noexcept
{
    const auto hash = block.hash();
    const auto key = to_siphash_key(slice<zero, to_half(hash_size)>(hash));
    data_stack scripts;

    for (const auto& tx: *block.transactions_ptr())
    {
        if (!tx->is_coinbase())
        {
            for (const auto& input: *(tx->inputs_ptr()))
            {
                if (!input->prevout->is_valid())
                    return false;

                const auto& script = input->prevout->script();

                if (!script.ops().empty())
                    scripts.push_back(script.to_data(false));
            }
        }

        for (const auto& output: *(tx->outputs_ptr()))
        {
            const auto& script = output->script();

            // bip138: any "nil" items MUST NOT be included.
            // bip138:exclude all outputs that start with OP_RETURN.
            if (!script.ops().empty() &&
                !chain::script::is_pay_op_return_pattern(script.ops()))
                scripts.push_back(script.to_data(false));
        }
    }

    // Order and remove duplicates.
    distinct(scripts);

    stream::out::data stream(out_filter);
    write::bytes::ostream writer(stream);
    writer.write_variable(scripts.size());
    golomb::construct(stream, scripts, golomb_bits, key, rate);
    stream.flush();

    return true;
}

hash_digest compute_filter_header(const hash_digest& previous_block_hash,
    const data_chunk& filter) noexcept
{
    return bitcoin_hash(splice(bitcoin_hash(filter), previous_block_hash));
}

bool match_filter(const block_filter& filter,
    const chain::script& script) noexcept
{
    if (script.ops().empty())
        return false;

    stream::in::copy stream(filter.filter);
    read::bytes::istream reader(stream);
    const auto set_size = reader.read_variable();

    if (!reader)
        return false;

    const auto target = script.to_data(false);
    const auto hash = slice<zero, to_half(hash_size)>(filter.hash);
    const auto key = to_siphash_key(hash);

    return golomb::match(target, stream, set_size, key, golomb_bits, rate);
}

bool match_filter(const block_filter& filter,
    const chain::scripts& scripts) noexcept
{
    if (scripts.empty())
        return false;

    data_stack stack;
    stack.reserve(scripts.size());

    std::for_each(scripts.begin(), scripts.end(),
        [&](const chain::script& script) noexcept
        {
            if (!script.ops().empty())
                stack.push_back(script.to_data(false));
        });

    if (stack.empty())
        return false;

    stream::in::copy stream(filter.filter);
    read::bytes::istream reader(stream);
    const auto set_size = reader.read_variable();

    if (!reader)
        return false;

    const auto hash = slice<zero, to_half(hash_size)>(filter.hash);
    const auto key = to_siphash_key(hash);

    stack.shrink_to_fit();
    return golomb::match(stack, stream, set_size, key, golomb_bits, rate);
}

bool match_filter(const block_filter& filter,
    const wallet::payment_address& address) noexcept
{
    return match_filter(filter, address.output_script());
}

bool match_filter(const block_filter& filter,
    const wallet::payment_address::list& addresses) noexcept
{
    if (addresses.empty())
        return false;

    static default_allocator<chain::script> no_fill_allocator{};
    chain::scripts stack(no_fill_allocator);
    stack.resize(addresses.size());

    std::transform(addresses.begin(), addresses.end(), stack.begin(),
        [](const wallet::payment_address& address) noexcept
        {
            return address.output_script();
        });

    return match_filter(filter, stack);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
