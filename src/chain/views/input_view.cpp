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
#include <bitcoin/system/chain/views/input_view.hpp>

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
namespace view {

constexpr auto point_size = chain::point::serialized_size();

// constructors
// ----------------------------------------------------------------------------

input::input() NOEXCEPT
{
}

input::input(const uint8_t* data, const uint8_t* witness,
    size_t witness_size) NOEXCEPT
  : data_{ data }, witness_{ witness }, witness_size_{ witness_size }
{
    const auto* position = std::next(data, point_size);
    size_ = possible_narrow_cast<size_t>(unsafe_from_variable(position));
    script_ = position;
}

// properties
// ----------------------------------------------------------------------------

bool input::is_valid() const NOEXCEPT
{
    return !is_null(data_);
}

const uint8_t* input::point_data() const NOEXCEPT
{
    return data_;
}

const hash_digest& input::point_hash() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return unsafe_array_cast<uint8_t, hash_size>(data_);
}

uint32_t input::point_index() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return unsafe_from_little_endian<uint32_t>(std::next(data_, hash_size));
}

bool input::is_null_point() const NOEXCEPT
{
    return point_hash() == null_hash && point_index() == point::null_index;
}

data_slice input::script_data() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return { script_, std::next(script_, size_) };
}

size_t input::script_size() const NOEXCEPT
{
    return size_;
}

data_slice input::witness_data() const NOEXCEPT
{
    if (is_null(witness_))
        return {};

    return { witness_, std::next(witness_, witness_size_) };
}

uint32_t input::sequence() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return unsafe_from_little_endian<uint32_t>(std::next(script_, size_));
}

bool input::is_final() const NOEXCEPT
{
    return sequence() == max_input_sequence;
}

const chain::script& input::script() const NOEXCEPT
{
    return *script_ptr();
}

const chain::script::cptr& input::script_ptr() const NOEXCEPT
{
    if (!script_ptr_)
        script_ptr_ = to_shared<chain::script>(script_data(), false);

    return script_ptr_;
}

const chain::witness& input::witness() const NOEXCEPT
{
    static const auto empty = to_shared<chain::witness>();

    if (!witness_ptr_)
        witness_ptr_ = is_null(witness_) ? empty :
            to_shared<chain::witness>(witness_data(), true);

    return *witness_ptr_;
}

// methods
// ----------------------------------------------------------------------------

bool input::is_roller() const NOEXCEPT
{
    return script().is_roller() || (!is_null(prevout) &&
        prevout->script().is_roller());
}

size_t input::signature_operations(bool bip16,
    bool bip141) const NOEXCEPT
{
    const auto factor = bip141 ? heavy_sigops_factor : one;
    const auto sigops = script().signature_operations(false) * factor;

    // Null prevout/input (coinbase) cannot have witness or embedded script.
    if (is_null(prevout))
        return sigops;

    chain::script embedded_witness;
    if (bip141 && witness().extract_sigop_script(embedded_witness,
        prevout->script()))
    {
        return ceilinged_add(sigops,
            embedded_witness.signature_operations(true));
    }

    chain::script embedded;
    if (bip16 && script().extract_sigop_script(embedded, prevout->script()))
    {
        if (bip141 && witness().extract_sigop_script(embedded_witness,
            embedded))
        {
            return ceilinged_add(sigops,
                embedded_witness.signature_operations(true));
        }

        return ceilinged_add(sigops,
            embedded.signature_operations(true) * factor);
    }

    return sigops;
}

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin
