/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/chain/views/output_view.hpp>

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
namespace view {

constexpr auto value_size = sizeof(uint64_t);

// constructors
// ----------------------------------------------------------------------------

output::output() NOEXCEPT
{
}

output::output(const uint8_t* data) NOEXCEPT
  : data_{ data }
{
    const auto* position = std::next(data, value_size);
    size_ = possible_narrow_cast<size_t>(unsafe_from_variable(position));
    script_ = position;
}

// serialization
// ----------------------------------------------------------------------------

void output::to_data(writer& sink) const NOEXCEPT
{
    sink.write_bytes(data_, serialized_size());
}

// properties
// ----------------------------------------------------------------------------

bool output::is_valid() const NOEXCEPT
{
    return !is_null(data_);
}

uint64_t output::value() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return unsafe_from_little_endian<uint64_t>(data_);
}

const uint8_t* output::data() const NOEXCEPT
{
    return data_;
}

data_slice output::script_data() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return { script_, std::next(script_, size_) };
}

size_t output::script_size() const NOEXCEPT
{
    return size_;
}

size_t output::serialized_size() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return std::distance(data_, script_) + size_;
}

const chain::script& output::script() const NOEXCEPT
{
    return *script_ptr();
}

const chain::script::cptr& output::script_ptr() const NOEXCEPT
{
    if (!ptr_)
        ptr_ = to_shared<chain::script>(script_data(), false);

    return ptr_;
}

// methods
// ----------------------------------------------------------------------------

hash_digest output::hash() const NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256::fast sink{ stream };
    to_data(sink);
    sink.flush();
    return out;
}

bool output::is_pay_op_return_pattern() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return !is_zero(size_) && (*script_ == to_value(opcode::op_return));
}

size_t output::signature_operations(bool bip141) const NOEXCEPT
{
    const auto factor = bip141 ? heavy_sigops_factor : one;
    stream::in::fast istream{ script_data() };
    read::bytes::fast source{ istream };
    return chain::script::signature_operations(source, false) * factor;
}

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin
