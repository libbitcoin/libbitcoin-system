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

#include <bitcoin/system/chain/compact_filter.hpp>

#include <initializer_list>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/message/messages.hpp>
#include <bitcoin/system/message/version.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

compact_filter compact_filter::factory(const data_chunk& data)
{
    compact_filter instance;
    instance.from_data(data);
    return instance;
}

compact_filter compact_filter::factory(std::istream& stream)
{
    compact_filter instance;
    instance.from_data(stream);
    return instance;
}

compact_filter compact_filter::factory(reader& source)
{
    compact_filter instance;
    instance.from_data(source);
    return instance;
}

compact_filter::compact_filter()
  : filter_type_(0u), block_hash_(null_hash), filter_()
{
}

compact_filter::compact_filter(uint8_t filter_type,
    const hash_digest& block_hash, const data_chunk& filter)
  : filter_type_(filter_type),
    block_hash_(block_hash),
    filter_(filter)
{
}

compact_filter::compact_filter(uint8_t filter_type, hash_digest&& block_hash,
    data_chunk&& filter)
  : filter_type_(filter_type),
    block_hash_(std::move(block_hash)),
    filter_(std::move(filter))
{
}

compact_filter::compact_filter(const compact_filter& other)
  : hash_(other.hash_cache()),
    filter_type_(other.filter_type_),
    block_hash_(other.block_hash_),
    filter_(other.filter_)
{
}

compact_filter::compact_filter(compact_filter&& other)
  : hash_(other.hash_cache()),
    filter_type_(other.filter_type_),
    block_hash_(std::move(other.block_hash_)),
    filter_(std::move(other.filter_))
{
}

bool compact_filter::is_valid() const
{
    return !filter_.empty();
}

void compact_filter::reset()
{
    filter_type_ = 0u;
    block_hash_.fill(0);
    filter_.clear();
    filter_.shrink_to_fit();
    invalidate_cache();
}

bool compact_filter::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool compact_filter::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool compact_filter::from_data(reader& source)
{
    reset();

    filter_type_ = source.read_byte();
    block_hash_ = source.read_hash();

    auto count = source.read_size_little_endian();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        filter_ = source.read_bytes(count);

    if (!source)
        reset();

    return source;
}

data_chunk compact_filter::to_data() const
{
    data_chunk data;
    const auto size = serialized_size();
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void compact_filter::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void compact_filter::to_data(writer& sink) const
{
    sink.write_byte(filter_type_);
    sink.write_hash(block_hash_);
    sink.write_variable_little_endian(filter_.size());
    sink.write_bytes(filter_);
}

size_t compact_filter::serialized_size() const
{
    return sizeof(filter_type_) + hash_size +
        message::variable_uint_size(filter_.size()) + filter_.size();
}

uint8_t compact_filter::filter_type() const
{
    return filter_type_;
}

void compact_filter::set_filter_type(uint8_t value)
{
    filter_type_ = value;
}

hash_digest& compact_filter::block_hash()
{
    return block_hash_;
}

const hash_digest& compact_filter::block_hash() const
{
    return block_hash_;
}

void compact_filter::set_block_hash(const hash_digest& value)
{
    block_hash_ = value;
}

void compact_filter::set_block_hash(hash_digest&& value)
{
    block_hash_ = std::move(value);
}

data_chunk& compact_filter::filter()
{
    return filter_;
}

const data_chunk& compact_filter::filter() const
{
    return filter_;
}

void compact_filter::set_filter(const data_chunk& value)
{
    filter_ = value;
}

void compact_filter::set_filter(data_chunk&& value)
{
    filter_ = std::move(value);
    invalidate_cache();
}

compact_filter& compact_filter::operator=(compact_filter&& other)
{
    hash_ = other.hash_cache();
    filter_type_ = other.filter_type_;
    block_hash_ = std::move(other.block_hash_);
    filter_ = std::move(other.filter_);
    return *this;
}

bool compact_filter::operator==(const compact_filter& other) const
{
    return (filter_type_ == other.filter_type_)
        && (block_hash_ == other.block_hash_)
        && (filter_ == other.filter_);
}

bool compact_filter::operator!=(const compact_filter& other) const
{
    return !(*this == other);
}

hash_digest compact_filter::get_header(hash_digest previous) const
{
    data_chunk data;

    {
        data_sink ostream(data);
        ostream_writer sink(ostream);
        sink.write_hash(hash());
        sink.write_hash(previous);
    }

    return bitcoin_hash(data);
}

hash_digest compact_filter::hash() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (!hash_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        hash_ = std::make_shared<hash_digest>(bitcoin_hash(filter()));
        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    const auto hash = *hash_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
}

// Cache.
//-----------------------------------------------------------------------------

// protected
void compact_filter::invalidate_cache() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (hash_)
    {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_.reset();
        //---------------------------------------------------------------------
        mutex_.unlock_and_lock_upgrade();
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

// Private cache access for copy/move construction.
compact_filter::hash_ptr compact_filter::hash_cache() const
{
    shared_lock lock(mutex_);
    return hash_;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
