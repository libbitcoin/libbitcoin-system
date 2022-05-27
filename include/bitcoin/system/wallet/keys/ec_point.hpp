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
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_POINT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_POINT_HPP

#include <cstdint>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/ec_scalar.hpp>

namespace libbitcoin {
namespace system {

/// ec_point is initialized to an invalid state.
/// Failed operations return an invalid state.
/// The bool operator reflects the validity state.
/// Does not implement string serialization.
class BC_API ec_point
{
public:
    static const uint8_t invalid;
    static const uint8_t compressed_even;
    static const uint8_t compressed_odd;
    static const uint8_t compressed_off;
    static const ec_point generator;

    /// Constructors.
    ec_point() noexcept;
    ec_point(ec_point&& point) noexcept;
    ec_point(const ec_point& point) noexcept;
    ec_point(ec_compressed&& compressed) noexcept;
    ec_point(const ec_compressed& compressed) noexcept;

    /// Operators.
    ec_point& operator=(ec_point&& point) noexcept;
    ec_point& operator=(const ec_point& point) noexcept;
    ec_point& operator=(ec_compressed&& compressed) noexcept;
    ec_point& operator=(const ec_compressed& compressed) noexcept;
    ec_point& operator+=(const ec_point& point) noexcept;
    ec_point& operator-=(const ec_point& point) noexcept;
    ec_point& operator*=(const ec_scalar& scalar) noexcept;
    ec_point operator-() const noexcept;

    /// Cast operators.
    operator bool() const noexcept;
    operator const ec_compressed&() const noexcept;

    /// Accessors.
    const ec_compressed& point() const noexcept;

private:
    bool is_valid() const noexcept;

    // This should be const, apart from the need to implement assignment.
    ec_compressed point_;
};

BC_API bool operator==(const ec_point& left, const ec_point& right) noexcept;
BC_API bool operator!=(const ec_point& left, const ec_point& right) noexcept;
BC_API ec_point operator+(const ec_point& left, const ec_point& right) noexcept;
BC_API ec_point operator-(const ec_point& left, const ec_point& right) noexcept;
BC_API ec_point operator*(const ec_point& left, const ec_scalar& right) noexcept;
BC_API ec_point operator*(const ec_scalar& left, const ec_point& right) noexcept;

} // namespace system
} // namespace libbitcoin

#endif

