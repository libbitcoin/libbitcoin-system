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
#include <bitcoin/system/message/inventory.hpp>

#include <algorithm>
#include <initializer_list>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/message/inventory.hpp>
#include <bitcoin/system/message/inventory_vector.hpp>
#include <bitcoin/system/message/messages.hpp>
#include <bitcoin/system/message/version.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace message {

const std::string inventory::command = "inv";
const uint32_t inventory::version_minimum = version::level::minimum;
const uint32_t inventory::version_maximum = version::level::maximum;

inventory inventory::factory(uint32_t version,
    const data_chunk& data)
{
    inventory instance;
    instance.from_data(version, data);
    return instance;
}

inventory inventory::factory(uint32_t version,
    std::istream& stream)
{
    inventory instance;
    instance.from_data(version, stream);
    return instance;
}

inventory inventory::factory(uint32_t version,
    reader& source)
{
    inventory instance;
    instance.from_data(version, source);
    return instance;
}

inventory::inventory()
  : inventories_()
{
}

inventory::inventory(const inventory_vector::list& values)
  : inventories_(values)
{
}

inventory::inventory(inventory_vector::list&& values)
  : inventories_(std::move(values))
{
}

inventory::inventory(const hash_list& hashes, type_id type)
{
    inventories_.clear();
    inventories_.reserve(hashes.size());
    const auto map = [type, this](const hash_digest& hash)
    {
        inventories_.emplace_back(type, hash);
    };

    std::for_each(hashes.begin(), hashes.end(), map);
}

inventory::inventory(const std::initializer_list<inventory_vector>& values)
  : inventories_(values)
{
}

inventory::inventory(const inventory& other)
  : inventory(other.inventories_)
{
}

inventory::inventory(inventory&& other)
  : inventory(std::move(other.inventories_))
{
}

bool inventory::is_valid() const
{
    return !inventories_.empty();
}

void inventory::reset()
{
    inventories_.clear();
    inventories_.shrink_to_fit();
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

    const auto count = source.read_size_little_endian();

    // Guard against potential for arbitary memory allocation.
    if (count > max_inventory)
        source.invalidate();
    else
        inventories_.resize(count);

    // Order is required.
    for (auto& inventory: inventories_)
        if (!inventory.from_data(version, source))
            break;

    if (!source)
        reset();

    return source;
}

data_chunk inventory::to_data(uint32_t version) const
{
    data_chunk data;
    const auto size = serialized_size(version);
    data.reserve(size);
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void inventory::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void inventory::to_data(uint32_t version, writer& sink) const
{
    sink.write_variable_little_endian(inventories_.size());

    for (const auto& inventory: inventories_)
        inventory.to_data(version, sink);
}

void inventory::to_hashes(hash_list& out, type_id type) const
{
    out.reserve(inventories_.size());

    for (const auto& element: inventories_)
        if (element.type() == type)
            out.push_back(element.hash());

    out.shrink_to_fit();
}

void inventory::reduce(inventory_vector::list& out, type_id type) const
{
    out.reserve(inventories_.size());

    for (const auto& inventory: inventories_)
        if (inventory.type() == type)
            out.push_back(inventory);

    out.shrink_to_fit();
}

size_t inventory::serialized_size(uint32_t version) const
{
    return variable_uint_size(inventories_.size()) +
        inventories_.size() * inventory_vector::satoshi_fixed_size(version);
}

size_t inventory::count(type_id type) const
{
    const auto is_type = [type](const inventory_vector& element)
    {
        return element.type() == type;
    };

    return count_if(inventories_.begin(), inventories_.end(), is_type);
}

inventory_vector::list& inventory::inventories()
{
    return inventories_;
}

const inventory_vector::list& inventory::inventories() const
{
    return inventories_;
}

void inventory::set_inventories(const inventory_vector::list& value)
{
    inventories_ = value;
}

void inventory::set_inventories(inventory_vector::list&& value)
{
    inventories_ = std::move(value);
}

inventory& inventory::operator=(inventory&& other)
{
    inventories_ = std::move(other.inventories_);
    return *this;
}

bool inventory::operator==(const inventory& other) const
{
    return (inventories_ == other.inventories_);
}

bool inventory::operator!=(const inventory& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace system
} // namespace libbitcoin
