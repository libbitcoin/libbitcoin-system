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
#ifndef LIBBITCOIN_EC_ARITHMETIC_HPP
#define LIBBITCOIN_EC_ARITHMETIC_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {

class ec_scalar
{
public:
    ec_scalar();
    ec_scalar(uint64_t value);
    ec_scalar(const ec_secret& secret);

    void reset();

    ec_scalar& operator=(uint64_t value);
    ec_scalar& operator=(const ec_secret& secret);

    bool is_zero() const;
    bool is_valid() const;
    operator bool() const;

    ec_scalar operator-() const;
    ec_scalar& operator+=(const ec_scalar& rhs);
    ec_scalar& operator-=(const ec_scalar& rhs);

    friend ec_scalar operator+(ec_scalar lhs, const ec_scalar& rhs);
    friend ec_scalar operator-(ec_scalar lhs, const ec_scalar& rhs);
    friend ec_scalar operator*(ec_scalar lhs, const ec_scalar& rhs);

    const ec_secret& secret() const;
    operator ec_secret() const;

private:
    void invalidate();

    bool valid_ = false;
    ec_secret scalar_;
};

ec_scalar operator+(ec_scalar lhs, const ec_scalar& rhs);
ec_scalar operator-(ec_scalar lhs, const ec_scalar& rhs);
ec_scalar operator*(ec_scalar lhs, const ec_scalar& rhs);

class ec_point
{
public:
    ec_point();
    ec_point(const std::string& hex);
    ec_point(const ec_compressed& point);

    ec_point& operator=(const std::string& hex);
    ec_point& operator=(const ec_compressed& point);

    bool is_valid() const;
    operator bool() const;

    ec_point operator-() const;
    ec_point& operator+=(const ec_point& rhs);
    ec_point& operator-=(const ec_point& rhs);

    friend ec_point operator+(ec_point lhs, const ec_point& rhs);
    friend ec_point operator-(ec_point lhs, const ec_point& rhs);
    friend ec_point operator*(ec_point lhs, const ec_scalar& rhs);

    const ec_compressed& point() const;
    operator ec_compressed() const;

    static const ec_point G;

private:
    void invalidate();

    static ec_point initialize_G();

    ec_compressed point_;
};

ec_point operator+(ec_point lhs, const ec_point& rhs);
ec_point operator-(ec_point lhs, const ec_point& rhs);
ec_point operator*(ec_point lhs, const ec_scalar& rhs);
ec_point operator*(const ec_scalar& lhs, ec_point rhs);

} // namespace libbitcoin

#endif

