/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/witness.hpp>

#include <algorithm>
#include <istream>
#include <iterator>
#include <numeric>
#include <utility>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/message/messages.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::machine;

// Constructors.
//-----------------------------------------------------------------------------

// A default instance is invalid (until modified).
witness::witness()
  : valid_(false)
{
}

witness::witness(witness&& other)
  : operations_(std::move(other.operations_)), valid_(other.valid_)
{
}

witness::witness(const witness& other)
  : operations_(other.operations_), valid_(other.valid_)
{
}

witness::witness(const operation::list& ops)
{
    from_operations(ops);
}

witness::witness(operation::list&& ops)
{
    from_operations(std::move(ops));
}

witness::witness(data_chunk&& encoded, bool prefix)
{
    from_data(encoded, prefix);
}

witness::witness(const data_chunk& encoded, bool prefix)
{
    from_data(encoded, prefix);
}

// Operators.
//-----------------------------------------------------------------------------

witness& witness::operator=(witness&& other)
{
    reset();
    operations_ = std::move(other.operations_);
    valid_ = other.valid_;
    return *this;
}

witness& witness::operator=(const witness& other)
{
    reset();
    operations_ = other.operations_;
    valid_ = other.valid_;
    return *this;
}
bool witness::operator==(const witness& other) const
{
    return operations_ == other.operations_;
}

bool witness::operator!=(const witness& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
witness witness::factory_from_data(const data_chunk& encoded, bool prefix)
{
    witness instance;
    instance.from_data(encoded, prefix);
    return instance;
}

// static
witness witness::factory_from_data(std::istream& stream, bool prefix)
{
    witness instance;
    instance.from_data(stream, prefix);
    return instance;
}

// static
witness witness::factory_from_data(reader& source, bool prefix)
{
    witness instance;
    instance.from_data(source, prefix);
    return instance;
}

bool witness::from_data(const data_chunk& encoded, bool prefix)
{
    data_source istream(encoded);
    return from_data(istream, prefix);
}

bool witness::from_data(std::istream& stream, bool prefix)
{
    istream_reader source(stream);
    return from_data(source, prefix);
}

// Prefixed data assumed valid here though caller may confirm with is_valid.
bool witness::from_data(reader& source, bool prefix)
{
    reset();
    valid_ = true;
    operation op;

    // TODO: optimize store serialization to avoid loop, reading data directly.
    if (prefix)
    {
        // Witness prefix is an op count, not a byte length (unlike script).
        // On the wire each witness is prefixed with number of tokens (bip144).
        for (auto count = source.read_size_little_endian(); count > 0; --count)
        {
            op.from_data(source, true);
            operations_.push_back(std::move(op));
        }
    }
    else
    {
        while (!source.is_exhausted())
        {
            op.from_data(source, true);
            operations_.push_back(std::move(op));
        }
    }

    if (!source)
        reset();

    return source;
}

void witness::from_operations(operation::list&& ops)
{
    ////reset();
    operations_ = std::move(ops);
    valid_ = script::is_push_only(operations_);
}

void witness::from_operations(const operation::list& ops)
{
    ////reset();
    operations_ = ops;
    valid_ = script::is_push_only(operations_);
}

// private/static
data_chunk witness::operations_to_data(const operation::list& ops)
{
    data_chunk out;
    const auto size = serialized_size(ops);
    out.reserve(size);
    const auto concatenate = [&out](const operation& op)
    {
        auto bytes = op.to_data(true);
        std::move(bytes.begin(), bytes.end(), std::back_inserter(out));
    };

    std::for_each(ops.begin(), ops.end(), concatenate);
    BITCOIN_ASSERT(out.size() == size);
    return out;
}

// private/static
size_t witness::serialized_size(const operation::list& ops)
{
    const auto op_size = [](size_t total, const operation& op)
    {
        return total + op.serialized_size(true);
    };

    return std::accumulate(ops.begin(), ops.end(), size_t(0), op_size);
}

// protected
void witness::reset()
{
    valid_ = false;
    operations_.clear();
    operations_.shrink_to_fit();
}

bool witness::is_valid() const
{
    // Witness validity is consistent with ops validity (unlike script).
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk witness::to_data(bool prefix) const
{
    data_chunk data;
    const auto size = serialized_size(prefix);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, prefix);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void witness::to_data(std::ostream& stream, bool prefix) const
{
    ostream_writer sink(stream);
    to_data(sink, prefix);
}

void witness::to_data(writer& sink, bool prefix) const
{
    // Witness prefix is an op count, not a byte length (unlike script).
    if (prefix)
        sink.write_size_little_endian(operations_.size());

    const auto serialize = [&sink](const operation& op)
    {
        op.to_data(sink, true);
    };

    // TODO: optimize store serialization to avoid loop, writing data directly.
    std::for_each(operations_.begin(), operations_.end(), serialize);
}

// Iteration.
//-----------------------------------------------------------------------------
// These are syntactic sugar that allow the caller to iterate ops directly.
// The first operations access must be method-based to guarantee the cache.

void witness::clear()
{
    reset();
}

bool witness::empty() const
{
    return operations().empty();
}

size_t witness::size() const
{
    return operations().size();
}

const operation& witness::front() const
{
    BITCOIN_ASSERT(!operations().empty());
    return operations().front();
}

const operation& witness::back() const
{
    BITCOIN_ASSERT(!operations().empty());
    return operations().back();
}

const operation& witness::operator[](size_t index) const
{
    BITCOIN_ASSERT(index < operations().size());
    return operations()[index];
}

operation::iterator witness::begin() const
{
    return operations().begin();
}

operation::iterator witness::end() const
{
    return operations().end();
}

// Properties (size).
//-----------------------------------------------------------------------------

size_t witness::serialized_size(bool prefix) const
{
    // Witness prefix is an op count, not a byte length (unlike script).
    return (prefix ? message::variable_uint_size(operations_.size()) : 0u)
        + serialized_size(operations_);
}

const operation::list& witness::operations() const
{
    return operations_;
}

} // namespace chain
} // namespace libbitcoin
