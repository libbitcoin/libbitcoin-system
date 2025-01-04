/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_CHECKED_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_CHECKED_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// TODO: use this template to define all checked wallet primitives.
// For example: typedef checked<1, short_hash_size, 4> payment;

/// Checked types are used for bitcoin payment addresses and other standards
/// that leverage the same technique. The behavior is unique to bitcoin.
/// This class is optimized to transport value_type (only) at no material
/// cost over a simple array, while simplifying parsing and checksumming.
template <size_t Prefix, size_t Payload,
    size_t Checksum = checksum_default_size>
class checked
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(checked);

    static constexpr size_t value_size = (Prefix + Payload + Checksum);

    typedef data_array<Prefix> prefix_type;
    typedef data_array<Payload> payload_type;
    typedef data_array<Checksum> checksum_type;
    typedef data_array<value_size> value_type;

    /// Constructors.

    checked() NOEXCEPT;
    checked(value_type&& value) NOEXCEPT;
    checked(const value_type& value) NOEXCEPT;

    /// Validity is guaranteed from this construction.
    checked(const prefix_type& prefix, const payload_type& payload) NOEXCEPT;

    /// Operators.

    operator bool() const NOEXCEPT;
    operator data_chunk() const NOEXCEPT;
    operator const value_type&() const NOEXCEPT;

    /// Properties.

    prefix_type prefix() const NOEXCEPT;
    payload_type payload() const NOEXCEPT;
    checksum_type checksum() const NOEXCEPT;
    const value_type& value() const NOEXCEPT;

private:
    static checked from_payload(const prefix_type& version,
        const payload_type& payload) NOEXCEPT;

    value_type value_;
};

/// Comparison operators.

template <size_t Prefix, size_t Payload,
    size_t Checksum = checksum_default_size>
bool operator==(
    const checked<Prefix, Payload, Checksum>& left,
    const checked<Prefix, Payload, Checksum>& right) NOEXCEPT
{
    return left.value() == right.value();
}

template <size_t Prefix, size_t Payload,
    size_t Checksum = checksum_default_size>
bool operator!=(
    const checked<Prefix, Payload, Checksum>& left,
    const checked<Prefix, Payload, Checksum>& right) NOEXCEPT
{
    return !(left == right);
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/wallet/addresses/checked.ipp>

#endif
