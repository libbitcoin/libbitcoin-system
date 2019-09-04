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

//#include <bitcoin/system/machine/opcode.hpp>
#include <bitcoin/system/math/golomb_coded_sets.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace neutrino {

data_chunk compute_filter(const chain::block& validated_block)
{
    bool incomplete_data = false;
    const auto hash = validated_block.hash();
    const auto key = to_numeric_key(slice<0, half_hash_size, hash_size>(hash));

    data_stack items;

    for (const auto tx : validated_block.transactions())
    {
        if (!tx.is_coinbase())
        {
            for (const auto input : tx.inputs())
            {
                const auto prevout = input.previous_output();
                if (prevout.metadata.cache.value() == chain::output::not_found)
                {
                    incomplete_data = true;
                    break;
                }

                const auto script = prevout.metadata.cache.script();
                const auto data = script.to_data(false);
                if (data.size() > 0)
                    items.emplace_back(data);
            }
        }

        if (incomplete_data)
            break;

        for (const auto output : tx.outputs())
        {
            const auto script = output.script();

            // is_unspendable was (script.front().code() != machine::opcode::return_)
            if ((script.size() > 0) && !script.is_unspendable())
            {
                const auto data = script.to_data(false);
                items.emplace_back(data);
            }
        }
    }

    // remove duplicates
    std::sort(items.begin(), items.end(), std::less<data_chunk>());
    auto last = std::unique(items.begin(), items.end());
    items.erase(last, items.end());

    data_chunk filter;

    if (!incomplete_data)
    {
        {
            data_sink stream(filter);
            ostream_writer writer(stream);
            writer.write_variable_little_endian(items.size());
            golomb::construct(writer, items, golomb_bit_parameter, key,
                golomb_target_false_positive_rate);
        }
    }

    return filter;
}

hash_digest compute_filter_header(const hash_digest& previous_block_hash,
    const data_chunk& filter)
{
    data_chunk data;

        {
            data_sink ostream(data);
            ostream_writer sink(ostream);
            sink.write_hash(bitcoin_hash(filter));
            sink.write_hash(previous_block_hash);
        }

        return bitcoin_hash(data);
}

bool match_filter(const message::compact_filter& filter,
    const chain::script& script)
{
    bool result = false;

    if (filter.filter_type() != neutrino_filter_type)
        return result;

    data_chunk target = script.to_data(false);

    if (target.size() == 0)
        return result;

    const auto key = to_numeric_key(slice<0, half_hash_size, hash_size>(
        filter.block_hash()));

    auto data = filter.filter();
    data_source stream(data);
    istream_reader reader(stream);
    auto set_size = reader.read_variable_little_endian();

    if (reader)
        result = golomb::match(target, reader, set_size, key,
            golomb_bit_parameter, golomb_target_false_positive_rate);

    return result;
}

bool match_filter(const message::compact_filter& filter,
    const chain::script::list& scripts)
{
    bool result = false;

    if (filter.filter_type() != neutrino_filter_type)
        return result;

    data_stack targets;
    for (auto script : scripts)
        if (script.serialized_size(false) > 0)
            targets.emplace_back(script.to_data(false));

    if (targets.size() == 0)
        return result;

    const auto key = to_numeric_key(slice<0, half_hash_size, hash_size>(
        filter.block_hash()));

    auto data = filter.filter();
    data_source stream(data);
    istream_reader reader(stream);
    auto set_size = reader.read_variable_little_endian();

    if (reader)
        result = golomb::match(targets, reader, set_size, key,
            golomb_bit_parameter, golomb_target_false_positive_rate);

    return result;
}

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address& address)
{
    if (filter.filter_type() != neutrino_filter_type)
        return false;

    return match_filter(filter, address.output_script());
}

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address::list& addresses)
{
    if (filter.filter_type() != neutrino_filter_type)
        return false;

    chain::script::list scripts;

    for (auto address : addresses)
        scripts.emplace_back(address.output_script());

    return match_filter(filter, scripts);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
