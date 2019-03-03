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
#ifndef LIBBITCOIN__CHAIN_POINT_VALUE_HPP
#define LIBBITCOIN__CHAIN_POINT_VALUE_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point_value.hpp>
#include <chain_point.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

/// A valued point, does not implement specialized serialization methods.
class BC_API chain_point_value
{
public:
//    typedef std::vector<point_value> list;

    // Constructors.
    //-------------------------------------------------------------------------

    chain_point_value();
//    point_value(point_value&& other);
    chain_point_value(const chain_point_value& other);
//    point_value(point&& instance, uint64_t value);
    chain_point_value(const point& instance, uint64_t value);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
//    point_value& operator=(point_value other);
    chain_point_value& assign(chain_point_value other);

//    bool operator==(const point_value& other) const;
    bool eq(const chain_point_value& other) const;
//    bool operator!=(const point_value& other) const;

    // Swap implementation required to properly handle base class.
    friend void libbitcoin::chain::swap(chain_point_value& left, chain_point_value& right);

    // Properties (accessors).
    //-------------------------------------------------------------------------

    uint64_t value() const;
    void set_value(uint64_t value);

public:
    chain::point_value getValue() {
        return value;
    }

    void setValue(chain::point_value value) {
        this->value = value;
    }
private:
    chain::point_value value;
//    uint64_t value_;
};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif
