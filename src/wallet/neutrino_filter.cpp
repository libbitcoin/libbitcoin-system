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

#include <bitcoin/system/utility/neutrino_filter.hpp>

#include <algorithm>
#include <bitcoin/system/math/golomb_coding.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/iostream.hpp>

namespace libbitcoin {
namespace system {
namespace neutrino {

bool compute_filter(const chain::block& validated_block, data_chunk& out_filter)
{
    const auto hash = validated_block.hash();
    const auto key = to_siphash_key(slice<0, half_hash_size, hash_size>(hash));
    data_stack items;

    for (const auto& tx: validated_block.transactions())
    {
        if (!tx.is_coinbase())
        {
            for (const auto& input: tx.inputs())
            {
                const auto& prevout = input.previous_output();
                if (!prevout.metadata.cache.is_valid())
                    return false;

                const auto& script = prevout.metadata.cache.script();
                if (!script.empty())
                    items.push_back(script.to_data(false));
            }
        }

        for (const auto& output: tx.outputs())
        {
            const auto& script = output.script();

            if (!script.empty() &&
                (script.front().code() != machine::opcode::return_))
                items.push_back(script.to_data(false));
        }
    }

    // Order and remove duplicates.
    items = distinct(std::move(items));

    data_sink stream(out_filter);
    ostream_writer writer(stream);
    writer.write_variable_little_endian(items.size());
    golomb::construct(writer, items, golomb_bits, key,
        golomb_target_false_positive_rate);
    stream.flush();

    return true;
}

hash_digest compute_filter_header(const hash_digest& previous_block_hash,
    const data_chunk& filter)
{
    data_chunk data;
    data_sink stream(data);
    ostream_writer sink(stream);
    sink.write_hash(bitcoin_hash(filter));
    sink.write_hash(previous_block_hash);
    stream.flush();

    return bitcoin_hash(data);
}

bool match_filter(const message::compact_filter& filter,
    const chain::script& script)
{
    if (script.empty() || filter.filter_type() != neutrino_filter_type)
        return false;

    data_source stream(filter.filter());
    istream_reader reader(stream);
    const auto set_size = reader.read_variable_little_endian();

    if (!reader)
        return false;

    const auto target = script.to_data(false);
    const auto key = to_siphash_key(slice<0, half_hash_size, hash_size>(
        filter.block_hash()));

    return golomb::match(target, reader, set_size, key, golomb_bits,
        golomb_target_false_positive_rate);
}

bool match_filter(const message::compact_filter& filter,
    const chain::script::list& scripts)
{
    if (scripts.empty() || filter.filter_type() != neutrino_filter_type)
        return false;

    data_stack stack;
    stack.reserve(scripts.size());

    std::for_each(scripts.begin(), scripts.end(),
        [&](const chain::script& script)
        {
            if (!script.empty())
                stack.push_back(script.to_data(false));
        });

    if (stack.empty())
        return false;

    data_source stream(filter.filter());
    istream_reader reader(stream);
    auto set_size = reader.read_variable_little_endian();

    if (!reader)
        return false;

    const auto key = to_siphash_key(slice<0, half_hash_size, hash_size>(
        filter.block_hash()));

    return golomb::match(stack, reader, set_size, key, golomb_bits,
        golomb_target_false_positive_rate);
}

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address& address)
{
    return match_filter(filter, address.output_script());
}

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address::list& addresses)
{
    if (addresses.empty() || filter.filter_type() != neutrino_filter_type)
        return false;

    chain::script::list stack;
    stack.reserve(addresses.size());

    std::for_each(addresses.begin(), addresses.end(),
        [&](const wallet::payment_address& address)
        {
            stack.push_back(address.output_script());
        });

    return match_filter(filter, stack);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
