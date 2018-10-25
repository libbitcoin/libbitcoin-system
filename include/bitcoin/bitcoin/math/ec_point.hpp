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
#ifndef LIBBITCOIN_EC_POINT_HPP
#define LIBBITCOIN_EC_POINT_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_scalar.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {

class BC_API ec_point
{
public:
    static const ec_point G;
    static const uint8_t invalid;
    static const uint8_t compressed_even;
    static const uint8_t compressed_odd;
    static const uint8_t uncompressed;

    /// Constructors.
    ec_point();
    ec_point(const ec_compressed& point);

    /// Operators.
    // TODO: add equality and inequality operators.
    ec_point operator-() const;
    ec_point& operator+=(const ec_point& point);
    ec_point& operator-=(const ec_point& point);
    ec_point& operator=(const ec_compressed& compressed);
    friend ec_point operator+(ec_point left, const ec_point& right);
    friend ec_point operator-(ec_point left, const ec_point& right);
    friend ec_point operator*(ec_point left, const ec_scalar& right);

    /// Cast operators.
    operator bool() const;
    operator ec_compressed() const;

    /// Accessors.
    const ec_compressed& point() const;

protected:
    // This should be const, apart from the need to implement assignment.
    ec_compressed point_;
};

ec_point operator+(ec_point left, const ec_point& right);
ec_point operator-(ec_point left, const ec_point& right);
ec_point operator*(ec_point left, const ec_scalar& right);
ec_point operator*(const ec_scalar& left, ec_point right);

} // namespace libbitcoin

#endif

