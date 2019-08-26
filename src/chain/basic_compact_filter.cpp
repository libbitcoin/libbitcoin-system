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

#include <bitcoin/system/chain/basic_compact_filter.hpp>

#include <bitcoin/system/machine/opcode.hpp>
#include <bitcoin/system/math/golomb_coded_sets.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

//#include <initializer_list>
//#include <bitcoin/system/math/limits.hpp>
//#include <bitcoin/system/message/messages.hpp>
//#include <bitcoin/system/message/version.hpp>


namespace libbitcoin {
namespace system {
namespace chain {

basic_compact_filter basic_compact_filter::factory(const data_chunk& data)
{
	basic_compact_filter instance;
    instance.from_data(data);
    return instance;
}

basic_compact_filter basic_compact_filter::factory(std::istream& stream)
{
	basic_compact_filter instance;
    instance.from_data(stream);
    return instance;
}

basic_compact_filter basic_compact_filter::factory(reader& source)
{
	basic_compact_filter instance;
    instance.from_data(source);
    return instance;
}

basic_compact_filter::basic_compact_filter()
  : compact_filter()
{
}

basic_compact_filter::basic_compact_filter(const hash_digest& block_hash,
	const data_chunk& filter)
  : compact_filter(basic_filter_type, block_hash, filter)
{
}

basic_compact_filter::basic_compact_filter(hash_digest&& block_hash,
	data_chunk&& filter)
  : compact_filter(basic_filter_type,
      std::move(block_hash), std::move(filter))
{
}

basic_compact_filter::basic_compact_filter(const basic_compact_filter& other)
  : compact_filter(other)
{
}

basic_compact_filter::basic_compact_filter(basic_compact_filter&& other)
  : compact_filter(std::move(other))
{
}

basic_compact_filter::basic_compact_filter(const compact_filter& other)
  : compact_filter(other)
{
    if (filter_type() != basic_filter_type)
        reset();
}

basic_compact_filter::basic_compact_filter(compact_filter&& other)
  : compact_filter(std::move(other))
{
    if (filter_type() != basic_filter_type)
        reset();
}

bool basic_compact_filter::is_valid() const
{
    return compact_filter::is_valid()
        && (filter_type() == basic_filter_type);
}

void basic_compact_filter::reset()
{
    compact_filter::reset();
    set_filter_type(basic_filter_type);
}

basic_compact_filter& basic_compact_filter::operator=(
    basic_compact_filter&& other)
{
    compact_filter::operator=(std::move(other));
    return *this;
}

basic_compact_filter& basic_compact_filter::operator=(compact_filter&& other)
{
    compact_filter::operator=(std::move(other));
    return *this;
}

bool basic_compact_filter::operator==(const basic_compact_filter& other) const
{
    return compact_filter::operator==(other);
}

bool basic_compact_filter::operator==(const compact_filter& other) const
{
    return compact_filter::operator==(other);
}

bool basic_compact_filter::operator!=(const basic_compact_filter& other) const
{
    return !(*this == other);
}

bool basic_compact_filter::operator!=(const compact_filter& other) const
{
    return !(*this == other);
}

bool basic_compact_filter::match(const wallet::payment_address address) const
{
    return match(address.output_script());
}

bool basic_compact_filter::match(const script script) const
{
    bool result = false;
    data_chunk target = script.to_data(false);

    if (target.size() == 0)
        return result;

    const auto key = to_numeric_key(slice<0, half_hash_size, hash_size>(
        block_hash()));

    auto data = filter();
    data_source stream(data);
    istream_reader reader(stream);
    auto set_size = reader.read_variable_little_endian();

    if (reader)
        result = gcs::match(target, reader, set_size, key,
            golomb_bit_parameter, golomb_target_false_positive_rate);

    return result;
}

bool basic_compact_filter::match(
    const wallet::payment_address::list addresses) const
{
    script::list scripts;

    for (auto address : addresses)
        scripts.emplace_back(address.output_script());

    return match(scripts);
}

bool basic_compact_filter::match(const script::list scripts) const
{
    bool result = false;

    data_stack targets;
    for (auto script : scripts)
        if (script.serialized_size(false) > 0)
            targets.emplace_back(script.to_data(false));

    if (targets.size() == 0)
        return result;

    const auto key = to_numeric_key(slice<0, half_hash_size, hash_size>(
        block_hash()));

    auto data = filter();
    data_source stream(data);
    istream_reader reader(stream);
    auto set_size = reader.read_variable_little_endian();

    if (reader)
        result = gcs::match(targets, reader, set_size, key,
            golomb_bit_parameter, golomb_target_false_positive_rate);

    return result;
}

bool basic_compact_filter::populate(const block validated_block)
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
                if (prevout.metadata.cache.value() == output::not_found)
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
            if ((script.size() > 0) &&
                (script.front().code() != machine::opcode::return_))
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

    if (!incomplete_data)
    {
        data_chunk filter;
        {
            data_sink stream(filter);
            ostream_writer writer(stream);
            writer.write_variable_little_endian(items.size());
            gcs::construct(writer, items, golomb_bit_parameter, key,
                golomb_target_false_positive_rate);
        }

        set_block_hash(hash);
        set_filter_type(basic_filter_type);
        set_filter(filter);
    }

    return !incomplete_data;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
