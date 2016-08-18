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
#include <bitcoin/bitcoin/message/inventory.hpp>

#include <algorithm>
#include <initializer_list>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string inventory::command = "inv";
const uint32_t inventory::version_minimum = version::level::minimum;
const uint32_t inventory::version_maximum = version::level::maximum;

inventory inventory::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    inventory instance;
    instance.from_data(version, data);
    return instance;
}

inventory inventory::factory_from_data(uint32_t version,
    std::istream& stream)
{
    inventory instance;
    instance.from_data(version, stream);
    return instance;
}

inventory inventory::factory_from_data(uint32_t version,
    reader& source)
{
    inventory instance;
    instance.from_data(version, source);
    return instance;
}

inventory::inventory()
{
}

inventory::inventory(const inventory_vector::list& values)
{
    inventories.insert(inventories.end(), values.begin(), values.end());
}

inventory::inventory(const hash_list& hashes, type_id type)
{
    const auto map = [type](const hash_digest& hash)
    {
        return inventory_vector{ type, hash };
    };

    inventories.resize(hashes.size());
    std::transform(hashes.begin(), hashes.end(), inventories.begin(), map);
}

inventory::inventory(const std::initializer_list<inventory_vector>& values)
{
    inventories.insert(inventories.end(), values.begin(), values.end());
}

bool inventory::is_valid() const
{
    return !inventories.empty();
}

void inventory::reset()
{
    inventories.clear();
    inventories.shrink_to_fit();
}

bool inventory::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool inventory::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool inventory::from_data(uint32_t version, reader& source)
{
    reset();
    const auto count = source.read_variable_uint_little_endian();
    auto result = static_cast<bool>(source);

    if (result)
    {
        inventories.resize(count);

        for (auto& inventory: inventories)
        {
            result = inventory.from_data(version, source);

            if (!result)
                break;
        }
    }

    if (!result)
        reset();

    return result;
}

data_chunk inventory::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void inventory::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void inventory::to_data(uint32_t version, writer& sink) const
{
    sink.write_variable_uint_little_endian(inventories.size());

    for (const auto& inventory: inventories)
        inventory.to_data(version, sink);
}

void inventory::to_hashes(hash_list& out, type_id type) const
{
    out.reserve(inventories.size());

    for (const auto& inventory: inventories)
        if (inventory.type == type)
            out.push_back(inventory.hash);

    out.shrink_to_fit();
}

void inventory::reduce(inventory_vector::list& out, type_id type) const
{
    out.reserve(inventories.size());

    for (const auto& inventory: inventories)
        if (inventory.type == type)
            out.push_back(inventory);

    out.shrink_to_fit();
}

uint64_t inventory::serialized_size(uint32_t version) const
{
    return variable_uint_size(inventories.size()) + inventories.size() *
        inventory_vector::satoshi_fixed_size(version);
}

size_t inventory::count(type_id type) const
{
    const auto is_type = [type](const inventory_vector& element)
    {
        return element.type == type;
    };

    return count_if(inventories.begin(), inventories.end(), is_type);
}

bool operator==(const inventory& left, const inventory& right)
{
    return left.inventories == right.inventories;
}

bool operator!=(const inventory& left, const inventory& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
