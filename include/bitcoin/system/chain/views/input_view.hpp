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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_INPUT_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_INPUT_VIEW_HPP

#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/views/output_view.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A view of a wire-encoded input and its witness within a block buffer.
class BC_API input_view final
{
public:
    DEFAULT_COPY_MOVE(input_view);

    /// Default view is invalid.
    input_view() NOEXCEPT;

    /// Data must point to the first byte of a wire-encoded input (point).
    /// Witness must point to the prefixed witness stack of the input (or null).
    input_view(const uint8_t* data, const uint8_t* witness,
        size_t witness_size) NOEXCEPT;

    /// Properties.
    bool is_valid() const NOEXCEPT;
    const uint8_t* point_data() const NOEXCEPT;
    const hash_digest& point_hash() const NOEXCEPT;
    uint32_t point_index() const NOEXCEPT;
    bool is_null_point() const NOEXCEPT;
    data_slice script_data() const NOEXCEPT;
    size_t script_size() const NOEXCEPT;
    data_slice witness_data() const NOEXCEPT;
    uint32_t sequence() const NOEXCEPT;
    bool is_final() const NOEXCEPT;

    /// Script and witness are deserialized on first use.
    const chain::script& script() const NOEXCEPT;
    const chain::script::cptr& script_ptr() const NOEXCEPT;
    const chain::witness& witness() const NOEXCEPT;

    /// Methods.
    bool is_roller() const NOEXCEPT;
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    /// Public mutable prevout, populated by block view (null if not).
    mutable const output_view* prevout{};

private:
    const uint8_t* data_{};
    const uint8_t* script_{};
    size_t size_{};
    const uint8_t* witness_{};
    size_t witness_size_{};
    mutable chain::script::cptr script_ptr_{};
    mutable chain::witness::cptr witness_ptr_{};
};

using input_views = std::vector<input_view>;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
