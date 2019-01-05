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
#include <bitcoin/system/math/ec_point.hpp>

#include <cstdint>
#include <bitcoin/system/formats/base_16.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/serializer.hpp>

namespace libbitcoin {
namespace system {

// TODO: use binary value.
#define LITERAL_G \
"0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"

const ec_point ec_point::G = base16_literal(LITERAL_G);
const uint8_t ec_point::invalid = 0x00;
const uint8_t ec_point::compressed_even = 0x02;
const uint8_t ec_point::compressed_odd = 0x03;
const uint8_t ec_point::uncompressed = 0x04;

// First point byte is used as validity sentinel.
////static_assert(null_compressed_point[0] == ec_point::invalid, "bad sentinel");

ec_point::ec_point()
  : point_(null_compressed_point)
{
}

ec_point::ec_point(const ec_compressed& point)
  : point_(point)
{
}

// Operators.
// ----------------------------------------------------------------------------

ec_point ec_point::operator-() const
{
    if (!(*this))
        return *this;

    auto negation = *this;
    if (!ec_negate(negation.point_))
        return {};

    return negation;
}

ec_point& ec_point::operator+=(const ec_point& point)
{
    if (!(*this))
        return *this;

    *this = *this + point;
    return *this;
}

ec_point& ec_point::operator-=(const ec_point& point)
{
    if (!(*this))
        return *this;

    *this = *this - point;
    return *this;
}

ec_point& ec_point::operator=(const ec_compressed& compressed)
{
    point_ = compressed;
    return *this;
}

ec_point operator+(ec_point left, const ec_point& right)
{
    if (!left || !right)
        return {};

    if (!ec_sum(left.point_, { left.point_, right.point_ }))
        return {};

    return left;
}

ec_point operator-(ec_point left, const ec_point& right)
{
    if (!left || !right)
        return {};

    const auto negative_right = -right;
    if (!negative_right)
        return {};

    return left + negative_right;
}

ec_point operator*(ec_point left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    if (!ec_multiply(left.point_, right.secret()))
        return {};

    return left;
}

ec_point operator*(const ec_scalar& left, ec_point right)
{
    return right * left;
}

ec_point::operator bool() const
{
    // First point byte is used as validity sentinel.
    return point_[0] != invalid;
}

ec_point::operator ec_compressed() const
{
    return point_;
}

const ec_compressed& ec_point::point() const
{
    return point_;
}

} // namespace system
} // namespace libbitcoin
