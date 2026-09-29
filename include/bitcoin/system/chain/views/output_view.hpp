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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_VIEW_HPP

#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A view of a wire-encoded output (value and prefixed script).
class BC_API output_view final
{
public:
    DEFAULT_COPY_MOVE(output_view);

    /// Default view is invalid.
    output_view() NOEXCEPT;

    /// Data must point to the first byte of a wire-encoded output.
    output_view(const uint8_t* data) NOEXCEPT;

    /// Serialization.
    void to_data(writer& sink) const NOEXCEPT;

    /// Properties.
    bool is_valid() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    const uint8_t* data() const NOEXCEPT;
    data_slice script_data() const NOEXCEPT;
    size_t script_size() const NOEXCEPT;
    size_t serialized_size() const NOEXCEPT;

    /// Script is deserialized on first use.
    const chain::script& script() const NOEXCEPT;
    const chain::script::cptr& script_ptr() const NOEXCEPT;

    /// Methods.
    hash_digest hash() const NOEXCEPT;
    bool is_pay_op_return_pattern() const NOEXCEPT;
    size_t signature_operations(bool bip141) const NOEXCEPT;

private:
    const uint8_t* data_{};
    const uint8_t* script_{};
    size_t size_{};
    mutable chain::script::cptr ptr_{};
};

using output_views = std::vector<output_view>;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
